#ifndef RPC_PROVIDER_H
#define RPC_PROVIDER_H
#include <memory>

#include "google/protobuf/service.h"
#include "muduo/net/TcpServer.h"
#include "muduo/net/EventLoop.h"
#include "muduo/net/InetAddress.h"
#include "muduo/net/TcpConnection.h"
#include "muduo/base/Timestamp.h"

// 框架提供的专用于发布 rpc 网络服务的对象类
class RpcProvider {
public:
  // 该方法是框架提供给外部使用的，是用来发布 rpc方法的接口，应当能接收任何一个 rpc 服务类，而 protobuf
  // 中生成的类都是从 google::protobuf::Service 继承而来，所以参数类型应为 Service 指针
  void NotifyService(::google::protobuf::Service *service);

  // 启动 rpc 服务节点，开始提供 rpc 远程网络调用服务
  void Run();

private:
  muduo::net::EventLoop eventLoop_;
  // 新的 socket 连接时的回调
  void OnConnection(const muduo::net::TcpConnectionPtr &conn);
  // 已建立连接用户的读写事件回调
  void OnMessage(const muduo::net::TcpConnectionPtr &conn,
                 muduo::net::Buffer *buf, muduo::Timestamp tm);
};


#endif