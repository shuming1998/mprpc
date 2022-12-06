#include <iostream>
#include <string>

#include "user.pb.h"
#include "rpcprovider.h"
#include "mprpcapplication.h"

// 本地服务，在当前进程中提供两个方法: Login、GetFriendLists
// fixbug::UserServiceRpc 使用在 rpc 发布端(即rpc服务的提供者)
class UserService : public fixbug::UserServiceRpc {
public:
  // 这是一个本地方法
  bool Login(std::string name, std::string pwd) {
    std::cout << "Local service: Login" << '\n';
    std::cout << "name: " << name << " pwd: " << pwd << '\n';
    return true;
  }

  /*
   * 下面这些方法都是框架直接调用的
   * 1. caller => Login(LoginRequest) => muduo => callee
   * 2. callee => Login(LoginRequest) => void Login(即下面重写的方法)
  */
  // 重写基类 fixbug::UserServiceRpc 中的虚函数
  void Login(::google::protobuf::RpcController* controller,
                       const ::fixbug::LoginRequest* request,
                       ::fixbug::LoginResponse* response,
                       ::google::protobuf::Closure* done) {
    // 框架给业务上报了反序列化后的请求参数 request，应用程序可以直接获取相应数据
    std::string name = request->name();
    std::string pwd = request->pwd();

    // 然后做本地业务
    bool loginResult = Login(name, pwd);

    // 将响应写入 response，包括错误码、错误消息、返回值
    fixbug::ResultCode *code = response->mutable_result();
    code->set_errcode(0);
    code->set_errmsg("");
    response->set_success(loginResult);

    // 执行回调操作：相应消息的序列化和网络发送(都由框架完成)
    done->Run();
  }

};

int main(int argc, char **argv) {
  // 首先调用框架的初始化操作
  MprpcApplication::Init(argc, argv);
  // provider 是一个 rpc 网络服务对象，把 UserService 对象发布到 rpc 节点上
  RpcProvider provider;
  provider.NotifyService(new UserService());
  // 启动一个 rpc 服务发布节点，进程进入阻塞状态，等待 rpc 方法被请求调用
  provider.Run();
  return 0;
}