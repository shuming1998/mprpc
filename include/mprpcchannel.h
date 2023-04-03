#ifndef MPRPC_CHANNEL_H
#define MPRPC_CHANNEL_H
#include <google/protobuf/service.h>
#include <google/protobuf/message.h>
#include <google/protobuf/service.h>
#include <google/protobuf/descriptor.h>

class MprpcChannel : public ::google::protobuf::RpcChannel {
public:
  // 所有 rpc caller 通过 stub 对象调用的 rpc 方法都会走向这个方法，在这里做数据的序列化和网络发送
  void CallMethod(const google::protobuf::MethodDescriptor *method,
                  google::protobuf::RpcController *controller,
                  const google::protobuf::Message *request,
                  google::protobuf::Message *response,
                  google::protobuf::Closure *done) override;
};

#endif