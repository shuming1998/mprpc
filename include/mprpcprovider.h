#ifndef MPRPC_PROVIDER_H
#define MPRPC_PROVIDER_H
#include <string>
#include <thread>
#include <iostream>
#include <functional>
#include <unordered_map>

#include "muduo/net/TcpServer.h"
#include "muduo/net/EventLoop.h"
#include "muduo/net/InetAddress.h"
#include "muduo/net/TcpConnection.h"
#include "muduo/base/Timestamp.h"
#include <google/protobuf/service.h>
#include <google/protobuf/descriptor.h>

// 框架提供的专用于发布 rpc 网络服务的对象类
class MprpcProvider {
public:
  // 该方法是框架提供给外部使用的，是用来发布 rpc 方法的接口，应当能接收任何一个 rpc 服务类，而 protobuf
  // 中生成的类都是从 google::protobuf::Service 继承而来，所以参数类型应为 Service 指针
  // 该接口中应有一张表，记录服务对象和其发布的所有服务方法
  void NotifyService(::google::protobuf::Service *service);
  // 启动 rpc 服务节点，开始提供 rpc 远程网络调用服务
  void Run();

private:
  // service 服务类型信息
  struct ServeceInfo {
    ::google::protobuf::Service *service_;  // 保存服务对象
    std::unordered_map<std::string, const ::google::protobuf::MethodDescriptor *> methodMap_;  // 保存服务方法
  };
  std::unordered_map<std::string, ServeceInfo> serviceMap_; // 保存已注册的服务对象和其所有服务方法的描述信息
  muduo::net::EventLoop eventLoop_;
  // 新的 socket 连接时的回调
  void OnConnection(const muduo::net::TcpConnectionPtr &conn);
  // 已建立连接用户的读写事件回调；当远程有调用 rpc 服务的请求时，OnMessage 方法就会响应
  void OnMessage(const muduo::net::TcpConnectionPtr &conn,
                 muduo::net::Buffer *buf, muduo::Timestamp tm);
  // Closure 的回调操作，序列化 rpc 方法调用结果的响应消息，然后通过网络发送出去
  void sendRpcResponse(const muduo::net::TcpConnectionPtr &conn, ::google::protobuf::Message *msg);
};


#endif