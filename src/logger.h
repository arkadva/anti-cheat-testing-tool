#include "utilities.h"
#include "debug.h"
#include <cstring>

#ifndef LOGGER_H_
#define LOGGER_H_

#define FILENAME (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)

#define LOG_BASE(level, message, ...) logger.level(utilities::string_format(message, ##__VA_ARGS__))
#define LOG_INFO(message, ...) LOG_BASE(info, message, ##__VA_ARGS__)
#define LOG_ERROR(message, ...) logger.error(utilities::string_format(std::string(message) + " %s::%s::%d", ##__VA_ARGS__, FILENAME, __FUNCTION__, __LINE__))
#define LOG_WARN(message, ...) LOG_BASE(warn, message, ##__VA_ARGS__)
#define LOG_CALL(message, ...) LOG_INFO(std::string("Function call: ") + __FUNCTION__ + "(" + message + ")", ##__VA_ARGS__)


#include <string>
#include <vector>

enum LogType {
  kNone = 0,
  kInfo = 1 << 0,
  kWarning = 1 << 1,
  kError = 1 << 2
};

// singleton logging class
class Logger {
public:
  static Logger& getInstance(int mask = kInfo | kWarning | kError);
  ~Logger();

  void info(const std::string& message);
  void warning(const std::string& message);
  void error(const std::string& message);

  Logger(const Logger&) = delete;
  Logger& operator=(const Logger&) = delete;

private:
  Logger(int mask);

  int logmask;
  std::vector<std::string> messages;
  void log(const std::string& message, LogType type);
};

#endif
