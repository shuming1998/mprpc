#include <iostream>
#include "user.pb.h"
#include "mprpcapplication.h"
#include "mprpcchannel.h"

int main(int argc, char **argv) {
  // 整个程序启动以后，想使用框架享受 rpc 服务调用，一定需要先调用框架的初始化函数(只初始化一次)
  MprpcApplication::Init(argc, argv);

  // 调用远程发布的 rpc 方法，必须传入一个 ::google::protobuf::RpcChannel* 来构造代理对象
  fixbug::UserServiceRpc_Stub stub(new MprpcChannel());
  // 调用 rpc 方法，实际上是调用 RpcChannel::callMethod，集中来做 rpc 方法的参数序列化和网络发送
  // 初始化 rpc 方法的请求参数
  fixbug::LoginRequest request;
  request.set_name("cuism");
  request.set_pwd("123456");
  // rpc 方法的响应，调用方只需要传参
  fixbug::LoginResponse response;
  // Login 会调用 MprpcChannel::callMethod，同步的 rpc 调用过程
  stub.Login(nullptr, &request, &response, nullptr);
  // 一次 rpc 调用完成，读取 response 结果，返回值的意义由用户设定
  if (0 == response.result().errcode()) {
    std::cout << "rpc Login response: " << response.success() << '\n';
  } else {
    std::cout << "rpc Login response error: " << response.result().errmsg() << '\n';
  }

  fixbug::RegistRequest req;
  req.set_id(6868);
  req.set_name("cui");
  req.set_pwd("666888");
  fixbug::RegistResponse res;
  stub.Register(nullptr, &req, &res, nullptr);

  // rpc 同步调用，等待返回结果
  if (0 == response.result().errcode()) {
    std::cout << "res regist response: " << response.success() << '\n';
  } else {
    std::cout << "res regist response error: " << response.result().errmsg() << '\n';
  }
  return 0;
}