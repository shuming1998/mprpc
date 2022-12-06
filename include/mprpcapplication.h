#ifndef MPRPC_APPLICATION_H
#define MPRPC_APPLICATION_H

#include "mprpcconfig.h"

// mprpc 框架基础类，类对象只需要实例化一次，故设计为单例
class MprpcApplication {
public:
  // 获取单例对象
  static MprpcApplication &getInstance();

  static void Init(int argc, char **argv);

  static MprpcConfig &GetConfig();


private:
  MprpcApplication() {}
  MprpcApplication(const MprpcApplication &) = delete;
  MprpcApplication(MprpcApplication &&) = delete;
  bool operator=(const MprpcApplication &) = delete;
  bool operator=(MprpcApplication &&) = delete;

  static MprpcConfig config_;
  static bool isInit_;  // 记录框架是否初始化
};




#endif