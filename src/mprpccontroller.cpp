#include "mprpccontroller.h"

RpcController::RpcController() : failed_(false), errText_("") {}

void RpcController::Reset() {
  failed_ = false;
  errText_ = "";
}

bool RpcController::Failed() const {
  return failed_;
}

std::string RpcController::ErrorText() const {
  return errText_;
}

void RpcController::SetFailed(const std::string &reason) {
  failed_ = true;
  errText_ = reason;
}

// 暂未实现具体的功能
void RpcController::StartCancel() {}
bool RpcController::IsCanceled() const { return false; }
void RpcController::NotifyOnCancel(::google::protobuf::Closure *callback) {}