#ifndef LOGGER_H
#define LOGGER_H
#include <string>
#include <stdio.h>
#include <iostream>

#include "lockqueue.h"

#define LOG_DEBUG(logmsgformat, ...)\
  do {\
    Logger &logger = Logger::getInstance();\
    logger.setLogLevel(DEBUG);\
    char c[1024] = {0};\
    snprintf(c, sizeof(c), logmsgformat, ##__VA_ARGS__);\
    logger.Log(c);\
  } while (0);

#define LOG_INFO(logmsgformat, ...)\
  do {\
    Logger &logger = Logger::getInstance();\
    logger.setLogLevel(INFO);\
    char c[1024] = {0};\
    snprintf(c, sizeof(c), logmsgformat, ##__VA_ARGS__);\
    logger.Log(c);\
  } while (0);

#define LOG_ERROR(logmsgformat, ...)\
  do {\
    Logger &logger = Logger::getInstance();\
    logger.setLogLevel(ERROR);\
    char c[1024] = {0};\
    snprintf(c, sizeof(c), logmsgformat, ##__VA_ARGS__);\
    logger.Log(c);\
  } while (0);

#define LOG_FATAL(logmsgformat, ...)\
  do {\
    Logger &logger = Logger::getInstance();\
    logger.setLogLevel(FATAL);\
    char c[1024] = {0};\
    snprintf(c, sizeof(c), logmsgformat, ##__VA_ARGS__);\
    logger.Log(c);\
  } while (0);

enum LogLevel {
  DEBUG,
  INFO,
  ERROR,
  FATAL,
};

// rpc 框架日志系统
class Logger {
public:
  // 获取日志单例
  static Logger &getInstance();
  // 设置日志级别
  void setLogLevel(LogLevel level);
  // 写日志
  void Log(std::string msg);

private:
  Logger();
  Logger(Logger &) = delete;
  Logger(const Logger &) = delete;
  bool operator=(Logger &) = delete;
  bool operator=(const Logger &) = delete;

  int logLevel_;  // 日志级别
  LockQueue<std::string> lockQueue_;  // 日志缓冲队列
};

#endif