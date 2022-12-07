#include <iostream>
#include <string>

#include "mprpcconfig.h"

// 解析加载配置文件
void MprpcConfig::LoadConfigFile(const char *configFile) {
  FILE *pf = fopen(configFile, "r");
  if (pf == nullptr) {
    std::cout << configFile << " doesn't exist!\n";
    exit(EXIT_FAILURE);
  }
  while (!feof(pf)) {
    char buf[512] = {0};
    fgets(buf, 512, pf);
    std::string formatStr(buf);
    // 处理配置行三种情况：1注释|2空格|3换行符
    formatStr = formatConfig(formatStr);
    // 跳过注释行和空行
    if (*formatStr.begin() == '#' || formatStr.empty()) {
      continue;
    }
    // 解析配置项
    int ind = formatStr.find('=');
    // 配置项不合法，没有等号
    if (-1 == ind) {
      continue;
    }
    std::string key;
    std::string value;
    key = formatStr.substr(0, ind);
    value = formatStr.substr(ind + 1, formatStr.size() - ind);
    // 将配置项写入 map
    configMap_.emplace(key, value);
  }
}

// 查询配置项信息
std::string MprpcConfig::Load(const std::string &key) {
  auto it = configMap_.find(key);
  return it == configMap_.end() ? "" : it->second;
}

// 处理配置文件的行
std::string MprpcConfig::formatConfig(std::string &str) {
  char format[str.size()] = {0};
  int id = 0;
  // 去掉所有的空格
  for (int i = 0; i < str.size(); ++i) {
    if (str[i] == ' ') {
      continue;
    }
    format[id++] = str[i];
  }

  std::string formatStr(format);
  id = formatStr.find_first_of(' ');
  if (id != -1) {
    formatStr = formatStr.substr(0, id);
  }

  id = formatStr.find_first_of('\0');
  if (id != -1) {
    formatStr = formatStr.substr(0, id);
  }

  // 去除当前行最后的换行符
  id = formatStr.find('\n');
  if (id != -1) {
    formatStr = formatStr.substr(0, id);
  }

  // 忽略有效行后面的注释: rpcserverip=127.0.0.1#rpc节点的ip地址
  id = formatStr.find_first_of('#');
  if (-1 != id && 0 != id) {
    // 截取不包含注释的有效行
    formatStr = formatStr.substr(0, id);
  }

  return formatStr;
}

