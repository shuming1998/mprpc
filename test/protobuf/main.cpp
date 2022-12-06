#include "test.pb.h"
#include <iostream>
#include <string>

using namespace fixbug;

int main() {
  LoginResponse rsp;
  // 对于 protobuf 消息中包含的消息，提供一个 mutable_xxx 方法获取该成员消息的指针
  ResultCode *rc = rsp.mutable_result();
  // 然后通过 set_xxx 方法设置值
  rc->set_errcode(1);
  rc->set_errmsg("登录处理失败");

  // 处理 protobuf 中的列表类型
  GetFriendListsResponse rspc;
  ResultCode *rpc = rspc.mutable_result();
  rpc->set_errcode(0);
  // 在列表中新增加一个对象，返回该对象的指针
  User *user1 = rspc.add_friend_list();
  // 通过相应的方法设置对象属性的值
  user1->set_name("zhang san");
  user1->set_age(20);
  user1->set_sex(User::MAN);
  User *user2 = rspc.add_friend_list();
  user2->set_name("li si");
  user2->set_age(20);
  user2->set_sex(User::MAN);
  // 打印列表中对象个数
  std::cout << rspc.friend_list_size() << '\n';
  // 访问列表中的第 n 个对象
  User user = rspc.friend_list(1);
  std::cout << user.name() << ' ' << user.age() << '\n';
  return 0;
}

int main1() {
  // 封装了 LoginRequest message 的对象
  LoginRequest req;
  req.set_name("zhang san");
  req.set_pwd("123456");

  // 序列化，将对象转化为字节流
  std::string send_str;
  if (req.SerializeToString(&send_str)) {
    std::cout << send_str << '\n';
  }

  // 从 send_str 反序列化一个 login 请求对象
  LoginRequest reqB;
  if (reqB.ParseFromString(send_str)) {
    std::cout << reqB.name() << '\n';
    std::cout << reqB.pwd() << '\n';
  }
  return 0;
}