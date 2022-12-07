#include <iostream>
#include <string>
#include <vector>

#include "friend.pb.h"
#include "mprpcapplication.h"
#include "mprpcprovider.h"
#include "logger.h"

class FriendService : public fixbug::FriendServiceRpc {
public:
  std::vector<std::string> GetFriendsList(uint32_t userId) {
    std::cout << "user id: " << userId << " do GetFriendsList service!\n";
    std::vector<std::string> res;
    res.push_back("zhang san");
    res.push_back("li si");
    res.push_back("wang wu");
    return res;
  }

  void GetFriendsList(::google::protobuf::RpcController *controller,
                      const ::fixbug::GetFriendsListRequest *request,
                      ::fixbug::GetFriendsListResponse *response,
                      ::google::protobuf::Closure *done) {
    uint32_t userId = request->uerid();
    std::vector<std::string> friendList = GetFriendsList(userId);
    fixbug::ResultCode *res = response->mutable_result();
    res->set_errcode(0);
    res->set_errmsg("");
    for (std::string &name : friendList) {
      std::string *p = response->add_friends();
      *p = name;
    }
    done->Run();
  }
};

int main(int argc, char **argv) {
  LOG_INFO("this is a info");
  LOG_ERROR("this is a error");
  // 调用框架的初始化操作
  MprpcApplication::Init(argc, argv);

  // provider是一个rpc网络服务对象。把UserService对象发布到rpc节点上
  MprpcProvider provider;
  provider.NotifyService(new FriendService());

  // 启动一个rpc服务发布节点   Run以后，进程进入阻塞状态，等待远程的rpc调用请求
  provider.Run();
  return 0;
}