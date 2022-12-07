#include <string>
#include <iostream>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "mprpcchannel.h"
#include "rpcheader.pb.h"
#include "mprpcapplication.h"

/*
 * 所有rpc caller 通过 stub 对象调用的 rpc 方法都会走向这个方法，在这里做|数据的序列化|和|网络发送|
 * headerSize + serviceName + methodName + paramSize + params
*/
void MprpcChannel::CallMethod(const ::google::protobuf::MethodDescriptor *method,
                              ::google::protobuf::RpcController *controller,
                              const ::google::protobuf::Message *request,
                              ::google::protobuf::Message *response,
                              ::google::protobuf::Closure *done) {
  const ::google::protobuf::ServiceDescriptor *sd = method->service();
  const std::string serviceName = sd->name();
  const std::string methodName = method->name();

  std::cout << "serviceName: " << serviceName << '\n';
  std::cout << "methodName: " << methodName << '\n';

  // 获取参数的序列化字符串长度
  uint32_t paramSize = 0;
  std::string paramStr;
  if (request->SerializeToString(&paramStr)) {
    paramSize = paramStr.size();
  } else {
    controller->SetFailed("serialize request error!");
    return;
  }

  // 定义 rpc 的请求 header
  mprpc::RpcHeader rpcHeader;
  rpcHeader.set_servicename(serviceName);
  rpcHeader.set_methodname(methodName);
  rpcHeader.set_paramsize(paramSize);

  // header 序列化
  uint32_t headerSize = 0;
  std::string rpcHeaderStr;
  if (rpcHeader.SerializeToString(&rpcHeaderStr)) {
    // 获取 header 长度
    headerSize = rpcHeaderStr.size();
  } else {
    controller->SetFailed("serialize rpc header error!");
    return;
  }
  // 组装待发送的 rpc 请求的字符串
  std::string sendRpcStr;
  sendRpcStr.insert(0, std::string((char*)&headerSize, 4));
  sendRpcStr += rpcHeaderStr;
  sendRpcStr += paramStr;

  std::cout << "================================\n";
  std::cout << "headerSize: " << headerSize << '\n';
  std::cout << "rpcHeaderStr: " << rpcHeaderStr << '\n';
  std::cout << "serviceName: " << serviceName << '\n';
  std::cout << "methodName: " << methodName << '\n';
  std::cout << "paramSize: " << paramSize << '\n';
  std::cout << "paramStr: " << paramStr << '\n';
  std::cout << "================================\n";

  // 使用 tcp 完成远程 rpc 方法的调用
  int cliendFd = socket(AF_INET, SOCK_STREAM, 0);
  if (-1 == cliendFd) {
    controller->SetFailed("create socket error!");
    exit(EXIT_FAILURE);
  }

  // 获取配置文件中的 ip 和端口号初始化结构体
  std::string ip = MprpcApplication::getInstance().GetConfig().Load("rpcserverip");
  uint16_t port = atoi(MprpcApplication::getInstance().GetConfig().Load("rpcserverport").c_str());

  sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  server_addr.sin_addr.s_addr = inet_addr(ip.c_str());

  // 连接 rpc 服务节点
  if (-1 == connect(cliendFd, (sockaddr *)&server_addr, sizeof(server_addr))) {
    char buffer[512] = {0};
    sprintf(buffer, "connect error! errno: %d", errno);
    controller->SetFailed(buffer);
    close(cliendFd);
    exit(EXIT_FAILURE);
  }

  // 发送 rpc 请求
  if (-1 == send(cliendFd, sendRpcStr.c_str(), sendRpcStr.size(), 0)) {
    char buffer[512] = {0};
    sprintf(buffer, "send sendRpcStr error! errno: %d", errno);
    controller->SetFailed(buffer);
    close(cliendFd);
    return;
  }

  // 接收 rpc 请求的响应
  char buf[1024] = {0};
  uint32_t receiveSize = 0;
  if (-1 == (receiveSize = recv(cliendFd, buf, sizeof(buf), 0))) {
    char buffer[512] = {0};
    sprintf(buffer, "recv error! errno: %d", errno);
    controller->SetFailed(buffer);
    close(cliendFd);
    return;
  }

  // 反序列化 rpc 调用的响应数据
  if (!response->ParseFromArray(buf, receiveSize)) {
    char buffer[2048] = {0};
    sprintf(buffer, "recv error! response str: %s", buf);
    controller->SetFailed(buffer);
    close(cliendFd);
    return;
  }
  close(cliendFd);
}
