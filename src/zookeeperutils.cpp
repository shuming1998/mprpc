#include "logger.h"
#include "zookeeperutils.h"
#include "mprpcapplication.h"

// watcher 观察器，zkserver 给 zkclient 的通知
void global_watcher(zhandle_t *zh, int type, int state, const char *path, void *watcherCtx) {
  // 回调的消息类型，和会话相关(连接、断开连接、超时等)
  if (type == ZOO_SESSION_EVENT) {
    // zkclient 和 zkserver 连接成功
    if (state == ZOO_CONNECTED_STATE) {
      // 从指定的句柄上获取信号量
      sem_t *sem = (sem_t *)zoo_get_context(zh);
      // 信号量资源 +1，主线程唤醒
      sem_post(sem);
    }
  }
}

ZkClient::ZkClient() : zhandle_(nullptr) {}
ZkClient::~ZkClient() {
  if (zhandle_ != nullptr) {
    // 关闭句柄，释放资源
    zookeeper_close(zhandle_);
  }
}

// 连接 zkserver
void ZkClient::Start() {
  std::string host = MprpcApplication::getInstance().GetConfig().Load("zookeeperip");
  std::string port = MprpcApplication::getInstance().GetConfig().Load("zookeeperport");
  std::string connStr = host + ":" + port;

  /*
   * zookeeper_mt 多线程版本
   * zookeeper 的 API 客户端程序提供了三个线程：
   * API 调用线程 | 网络 I/O 线程 | watcher 回调线程
  */

  // zookeeper_init 是一个异步连接方法，正确返回仅代表创建句柄成功，连接 zkserver 会话是否成功未知
  zhandle_ = zookeeper_init(connStr.c_str(), global_watcher, 30000, nullptr, nullptr, 0);
  if (nullptr == zhandle_) {
    std::cout << "zookeeper_init error!\n";
    exit(EXIT_FAILURE);
  }

  // 向句柄中设置一个信号量（可以设置多个数据）
  sem_t sem;
  sem_init(&sem, 0, 0);
  zoo_set_context(zhandle_, &sem);

  // 主线程阻塞，等待 zkserver 的回调函数 global_watcher 响应
  sem_wait(&sem);
  std::cout << "zookeeper_init success!\n";
}


void ZkClient::Create(const char *path, const char *data, int dataLen, int state) {
  char pathBuf[128];
  int bufferLen = sizeof(pathBuf);
  int flag;
  // 判断节点是否已经创建
  flag = zoo_exists(zhandle_, path, 0, nullptr);
  if (ZNONODE == flag) {
    // 根据指定 path 创建 znode 节点 | 句柄、存储路径、数据、数据长度、权限、节点类型、缓冲区、缓冲区长度
    flag = zoo_create(zhandle_, path, data, dataLen,
                      &ZOO_OPEN_ACL_UNSAFE, state, pathBuf, bufferLen);
    if (flag != ZOK) {
      std::cout << "flag: " << flag << '\n';
      std::cout << "znode create error! path: " << path << '\n';
      exit(EXIT_FAILURE);
    }
    std::cout << "znode create success! path: " << path << '\n';
  }
}

// 根据参数指定的 znode 节点路径回去节点中存储的值
std::string ZkClient::GetData(const char *path) {
  char buffer[64];
  int bufferLen = sizeof(buffer);
  int flag = zoo_get(zhandle_, path, 0, buffer, &bufferLen, nullptr);
  if (flag != ZOK) {
    std::cout << "get znode error! path: " << path << '\n';
    return "";
  }
  return buffer;
}