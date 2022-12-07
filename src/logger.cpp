#include <time.h>
#include <iostream>
#include "logger.h"

Logger::Logger() {
  // 启动写日志线程，将日志从 lockqueue 缓冲区写入磁盘
  std::thread writeLogTask([&]() {
    for (;;) {
      // 获取日期，取日志信息，追加写入日志文件 a+
      time_t now = time(nullptr);
      tm *nowTm = localtime(&now);

      char fileName[128];
      sprintf(fileName, "%d-%d-%d-log.txt", nowTm->tm_year + 1900, nowTm->tm_mon + 1, nowTm->tm_mday);
      FILE *pf = fopen(fileName, "a+");
      if (pf == nullptr) {
        std::cout << fileName << " open error!\n";
        exit(EXIT_FAILURE);
      }
      std::string msg = lockQueue_.Pop();
      char timeBuf[256] = {0};
      sprintf(timeBuf,
              "[%d:%d:%d]=>[%s] [%s--line:%d]:\n",
              nowTm->tm_hour, nowTm->tm_min,
              nowTm->tm_sec,
              (logLevel_ == INFO ? "INFO" : "ERROR"),
              __FILE__,
              __LINE__);
      msg.insert(0, timeBuf);
      msg.append("\n");
      fputs(msg.c_str(), pf);
      fclose(pf);
    }
  });
  writeLogTask.detach();
}

// 获取单例对象
Logger &Logger::getInstance() {
  static Logger logger;
  return logger;
}

// 设置日志级别
void Logger::setLogLevel(LogLevel level) {
  logLevel_ = level;
}

// 写日志，将日志信息写入到 lockQueu 缓冲区
void Logger::Log(std::string msg) {
  lockQueue_.Push(msg);
}