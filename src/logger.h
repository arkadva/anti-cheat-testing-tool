#ifndef LOGGER_H_
#define LOGGER_H_

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
