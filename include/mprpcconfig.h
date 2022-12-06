#ifndef MPRPC_CONFIG_H
#define MPRPC_CONFIG_H

#include <unordered_map>
#include <string>

// 框架中读取配置文件的类
class MprpcConfig {
public:
  // 解析加载配置文件
  void LoadConfigFile(const char *configFile);
  // 查询配置项信息
  std::string Load(const std::string &key);

private:
  std::unordered_map<std::string, std::string> configMap_;
  // 处理配置行三种情况：1注释|2空格|3换行符
  std::string formatConfig(std::string &str);
};

#endif