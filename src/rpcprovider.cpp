#include <string>
#include <thread>
#include <functional>

#include "rpcprovider.h"
#include "mprpcapplication.h"


void RpcProvider::NotifyService(::google::protobuf::Service *service) {

}

void RpcProvider::OnConnection(const muduo::net::TcpConnectionPtr &conn) {

}

void RpcProvider::OnMessage(const muduo::net::TcpConnectionPtr &conn,
                            muduo::net::Buffer *buf,
                            muduo::Timestamp tm) {

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