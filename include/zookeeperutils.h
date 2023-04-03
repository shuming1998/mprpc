#ifndef ZOOKEEPER_UTILS_H
#define ZOOKEEPER_UTILS_H

#include <string>
#include <iostream>
#include <semaphore.h>
#include <zookeeper/zookeeper.h>

// 封装的 zk 客户端类
class ZkClient {
public:
  ZkClient();
  ~ZkClient();
  // zkclient 启动连接 zkserver
  void Start();
  // 在 zkserver 上根据指定的 path 创建 znode 节点，这里默认 state = 0 为永久性节点
  void Create(const char *path, const char *data, int dataLen, int state = 0);
  // 根据参数指定的 znode 节点路径返回该节点中存储的值
  std::string GetData(const char *path);

private:
  // zk 的客户端句柄
  zhandle_t *zhandle_;
};

#endif