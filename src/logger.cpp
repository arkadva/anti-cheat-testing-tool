#include "Logger.h"
#include <fstream>
#include <iostream>

// ANSI color codes
#define RESET "\033[0m"
#define RED "\033[31m"    // error
#define YELLOW "\033[33m" // warning
#define BLUE "\033[34m"   // info

Logger::Logger(int mask) : logmask(mask) {}

Logger& Logger::getInstance(int mask) {
  static Logger instance(mask); // static instance of logger
  return instance;
}

Logger::~Logger() {
  std::ofstream log_file("logs.txt");
  if (log_file.is_open()) {
    for (const auto& msg : messages) {
      log_file << msg << std::endl;
    }
    std::cout << "Logs saved to logs.txt." << std::endl;
  }
  else {
    std::cerr << "Error opening logs.txt for writing." << std::endl;
  }
}

void Logger::info(const std::string& message) {
  log("INFO: " + message, kInfo);
}

void Logger::warning(const std::string& message) {
  log("WARNING: " + message, kWarning);
}

void Logger::error(const std::string& message) {
  log("ERROR: " + message, kError);
}

void Logger::log(const std::string& message, LogType type) {
  if (type & logmask) {
    std::string message;
    switch (type) {
    case kInfo:
      message = std::string(BLUE) + message + RESET;
      break;
    case kWarning:
      message = std::string(YELLOW) + message + RESET;
      break;
    case kError:
      message = std::string(RED) + message + RESET;
      break;
    default:
      message = message;
    }
    std::cout << message << std::endl;
    messages.push_back(message);
  }
}
