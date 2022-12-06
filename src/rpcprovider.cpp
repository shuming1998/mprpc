#include "rpcprovider.h"
#include "mprpcapplication.h"
#include "rpcheader.pb.h"

/*
service_name <=> service 描述  => service* 记录服务对象
                              => method_name => method 方法对象
*/
void RpcProvider::NotifyService(::google::protobuf::Service *service) {
  // 服务的所有描述信息结构体
  ServeceInfo serveceInfo;
  // 获取服务对象的描述信息
  const ::google::protobuf::ServiceDescriptor *pServiceDesc = service->GetDescriptor();
  // 获取服务的名字
  std::string serviceName = pServiceDesc->name();
  // std::cout << "serviceName: " << serviceName << '\n';
  // 获取服务对象 service 方法的数量
  int methodCnt = pServiceDesc->method_count();
  for (int i = 0; i < methodCnt; ++i) {
    // 获取服务对象指定下标的服务方法的描述
    const google::protobuf::MethodDescriptor *pMethodDesc = pServiceDesc->method(i);
    std::string methodName = pMethodDesc->name();
    // 记录方法名和方法的描述信息
    serveceInfo.methodMap_.emplace(methodName, pMethodDesc);
    // std::cout << "methodName: " << methodName << '\n';
  }
  // 记录服务
  serveceInfo.service_ = service;
  // 记录服务名和服务的描述信息
  serviceMap_.emplace(serviceName, serveceInfo);
}

void RpcProvider::OnConnection(const muduo::net::TcpConnectionPtr &conn) {
  if (!conn->connected()) {
    // 与 rpc client 的连接已断开，关闭连接
    conn->shutdown();
  }
}

// 在框架中，RpcProvider 和 RpcConsumer 应事先协商好双方通信时使用的 protobuf 数据类型
// 包括：serviceName、methodName、params <==> 定义 proto 的 message 类型，进行数据头的序列化/反序列化
// header_size(4 bytes) + header_str + params_str
void RpcProvider::OnMessage(const muduo::net::TcpConnectionPtr &conn,
                            muduo::net::Buffer *buf,
                            muduo::Timestamp tm) {
  // 从网络上接收的远程 rpc 调用请求的字节流
  std::string recvBuf = buf->retrieveAllAsString();
  // 从字符流中读取前 4 个字节
  uint32_t headerSize = 0;
  recvBuf.copy((char *)&headerSize, 4, 0);
  // 根据 header_size 读取数据头的原始字符流，将其反序列化为 rpc 请求的详细信息
  std::string rpcHeaderStr = recvBuf.substr(4, headerSize);
  mprpc::RpcHeader rpcHeader;
  std::string serviceName;
  std::string methodName;
  uint32_t paramSize;
  if (rpcHeader.ParseFromString(rpcHeaderStr)) {
    // 数据反序列化成功，读取 protobuf 中反序列化的数据
    serviceName = rpcHeader.servicename();
    methodName = rpcHeader.methodname();
    paramSize = rpcHeader.paramsize();
  } else {
    // 数据反序列化失败
    std::cout << "parse rpc_header_str data error!\n";
    return;
  }
  std::string paramStr = recvBuf.substr(headerSize + 4, paramSize);

  std::cout << "================================\n";
  std::cout << "headerSize: " << headerSize << '\n';
  std::cout << "rpcHeaderStr: " << rpcHeaderStr << '\n';
  std::cout << "serviceName: " << serviceName << '\n';
  std::cout << "methodName: " << methodName << '\n';
  std::cout << "paramSize: " << paramSize << '\n';
  std::cout << "paramStr: " << paramStr << '\n';
  std::cout << "================================\n";

  // 获取 service 对象和 message 对象
  auto it = serviceMap_.find(serviceName);
  if (it == serviceMap_.end()) {
    std::cout << "serviceName doesn't exist!\n";
    return;
  }
  auto itMd = it->second.methodMap_.find(methodName);
  if (itMd == it->second.methodMap_.end()) {
    std::cout << serviceName << " doesn't have method " << methodName << "!\n";
    return;
  }
  ::google::protobuf::Service *service = it->second.service_;   // 获取 service 对象
  const ::google::protobuf::MethodDescriptor *method = itMd->second;  // 获取 method 对象

  // 生成 rpc 方法调用的请求 request 和 响应 response 参数
  ::google::protobuf::Message *request = service->GetRequestPrototype(method).New();
  if (!request->ParseFromString(paramStr)) {
    std::cout << "request parse error!\n" << paramStr << '\n';
    return;
  }
  // 响应类型
  ::google::protobuf::Message *response = service->GetResponsePrototype(method).New();
  // 给下面的 method 方法调用绑定一个 google::protobuf::Closure 回调函数
  ::google::protobuf::Closure *done =
      google::protobuf::NewCallback<RpcProvider,
                                    const muduo::net::TcpConnectionPtr &,
                                    google::protobuf::Message *>
                                    (this,
                                    &RpcProvider::sendRpcResponse,
                                    conn,
                                    response);
  // 在框架上根据远程 rpc 请求调用当前节点上发布的 rpc 方法(调用的就是服务端中生成的 rpc 方法)
  service->CallMethod(method, nullptr, request, response, done);
}

void RpcProvider::Run() {
  // 获取配置文件中的 ip 和端口号初始化结构体
  std::string ip = MprpcApplication::getInstance().GetConfig().Load("rpcserverip");
  uint16_t port = atoi(MprpcApplication::getInstance().GetConfig().Load("rpcserverport").c_str());
  muduo::net::InetAddress address(ip, port);

  // 为了方便用户使用框架，在 Run 方法中封装 muduo
  // 创建 TcpServer 对象
  muduo::net::TcpServer tcpServer_(&eventLoop_, address, "RpcProvider");
  // 绑定连接回调和消息读写回调方法
  tcpServer_.setConnectionCallback(std::bind(&RpcProvider::OnConnection, this, std::placeholders::_1));
  tcpServer_.setMessageCallback(std::bind(&RpcProvider::OnMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
  // 设置 muduo 库的线程数
  tcpServer_.setThreadNum(std::thread::hardware_concurrency());
  // 启动网络服务
  tcpServer_.start();
  eventLoop_.loop();
}

// Closure 的回调操作，用于序列化 rpc 的响应和网络发送
// rpc 函数中的 done->run() 运行的就是这个绑定的方法
void RpcProvider::sendRpcResponse(const muduo::net::TcpConnectionPtr &conn, ::google::protobuf::Message *response) {
  // 序列化
  std::string responseStr;
  if (response->SerializeToString(&responseStr)) {
    // 序列化成功，通过网络将 rpc 方法执行的结果返回给 rpc 方法的调用方
    conn->send(responseStr);
  } else {
    std::cout << "serialize responseStr error!\n";
  }
  conn->shutdown(); // 模拟 http 的短连接服务，由 rpcProvider 主动断开连接
}