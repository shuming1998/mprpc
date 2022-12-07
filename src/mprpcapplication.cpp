#include <string>
#include <unistd.h>
#include <iostream>

#include "mprpcapplication.h"

bool MprpcApplication::isInit_ = false;  // 记录框架是否初始化
MprpcConfig MprpcApplication::config_;

void ShowArgsHelp() {
  std::cout << "format: command -i <configfile>\n";
}

void MprpcApplication::Init(int argc, char **argv) {
  if (isInit_) {
    return;
  }
  if (argc < 2) {
    ShowArgsHelp();
    exit(EXIT_FAILURE);
  }
  int c = 0;
  std::string configFile;
  /***************************************************************
  *  @brief int getopt(int argc, char * const argv[], const char *optstring): parse command params
  *  @param number of params
  *  @param list of params
  *  @param 包含正确的参数选项字符串：
  *  1.单个字符，表示选项，
  *  2.单个字符后接一个冒号：表示该选项后必须跟一个参数。参数紧跟在选项后或者以空格隔开。该参数的指针赋给optarg。
  *  3 单个字符后跟两个冒号，表示该选项后必须跟一个参数。参数必须紧跟在选项后不能以空格隔开。该参数的指针赋给optarg
  *  如 "ab:"，-a 为普通选项，-b 为有参数的选项，因为它后面有一个冒号
  *  外部变量说明：
  *  optarg：如果某个选项有参数，这包含当前选项的参数字符串
  *  optind：argv 的当前索引值
  *  opterr：正常运行状态下为 0。非零时表示存在无效选项或者缺少选项参数，并输出其错误信息
  *  optopt：当发现无效选项字符时，即 getopt() 方法返回 ? 字符，optopt 中包含的就是发现的无效选项字符
  **************************************************************/
  while ((c = getopt(argc, argv, "i:")) != -1) {
    switch (c) {
      case 'i':
        configFile = optarg;
        break;
      case '?':
        std::cout << "invalid args!\n";
        ShowArgsHelp();
        exit(EXIT_FAILURE);
      case ':':
        // 出现了 -i 但没有参数
        ShowArgsHelp();
        exit(EXIT_FAILURE);
      default:
        break;
    }
  }
  // 加载配置文件
  config_.LoadConfigFile(configFile.c_str());
  // #define TEST
  #ifdef TEST
  std::cout << "rpcserverip: " << config_.Load("rpcserverip") << '\n';
  std::cout << "rpcserverport: " << config_.Load("rpcserverport") << '\n';
  std::cout << "zookeeperip: " << config_.Load("zookeeperip") << '\n';
  std::cout << "zookeeperport: " << config_.Load("zookeeperport") << '\n';
  #endif
  isInit_ = true;
}

MprpcApplication &MprpcApplication::getInstance() {
  static MprpcApplication mprpcApp;
  return mprpcApp;
}


MprpcConfig &MprpcApplication::GetConfig() {
  return config_;
}