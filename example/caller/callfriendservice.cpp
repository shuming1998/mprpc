#include "friend.pb.h"
#include "mprpcapplication.h"
#include <iostream>

int main(int argc, char **argv) {
  // 整个程序启动以后，想使用框架享受 rpc
  // 服务调用，一定需要先调用框架的初始化函数(只初始化一次)
  MprpcApplication::Init(argc, argv);

  // 调用远程发布的 rpc 方法，必须传入一个 ::google::protobuf::RpcChannel*
  // 来构造代理对象
  fixbug::FriendServiceRpc_Stub stub(new MprpcChannel());
  // 调用 rpc 方法，实际上是调用 RpcChannel::callMethod，集中来做 rpc
  // 方法的参数序列化和网络发送 初始化 rpc 方法的请求参数
  fixbug::GetFriendsListRequest request;
  request.set_uerid(1);
  // rpc 方法的响应，调用方只需要传参
  fixbug::GetFriendsListResponse response;
  // 控制信息对象
  RpcController controller;
  // Login 会调用 MprpcChannel::callMethod，同步的 rpc 调用过程
  stub.GetFriendsList(&controller, &request, &response, nullptr);
  if (controller.Failed()) {
    std::cout << controller.ErrorText() << '\n';
  } else {
    // 一次 rpc 调用完成，读取 response 结果，返回值的意义由用户设定
    if (0 == response.result().errcode()) {
      std::cout << "rpc GetFriendList success!\n";
      int num = response.friends_size();
      for (int i = 0; i < num; ++i) {
        std::cout << "name: " << response.friends(i) << '\n';
      }
    } else {
      std::cout << "rpc GetFriendList error: " << response.result().errmsg() << '\n';
    }
  }
  return 0;
}