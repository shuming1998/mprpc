#ifndef MPRPC_CONTROLLER_H
#define MPRPC_CONTROLLER_H
#include <google/protobuf/service.h>
#include <string>

class RpcController : public ::google::protobuf::RpcController {
public:
  RpcController();
  void Reset() override;
  bool Failed() const override;
  std::string ErrorText() const override;
  void SetFailed(const std::string &reason) override;

  // 暂未实现具体的功能
  void StartCancel() override;
  bool IsCanceled() const override ;
  void NotifyOnCancel(::google::protobuf::Closure *callback) override;

private:
  bool failed_;
  std::string errText_;
};

#endif