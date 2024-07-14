#include <fstream>
#include <iostream>
#include "logger.h"
#include <conio.h>

// ANSI color codes
#define RESET "\033[0m"
#define RED "\033[31m"          // error
#define YELLOW "\033[33m"       // warning
#define WHITE "\033[37m"        // info
#define BOLD_PURPLE "\033[36m"  // confirmation

Logger::Logger(int displayMask, int saveMask) : displaymask_(displayMask), savemask_(saveMask) {}

Logger& Logger::getInstance(int diplayMask, int saveMask) {
  static Logger instance(diplayMask, saveMask); // static instance of logger
  return instance;
}

Logger::~Logger() {
  // FIXED ~~delete ANSI colors first (or store them seperately)~~

  /* 
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
  */
}

void Logger::confirmation(const std::string& message) {

  if (!message.empty()) {
    log(message, kConfirmation);
  }

  log("Press any key to continue...", kConfirmation);
  _getch();
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
  if (type & savemask_) {
    messages.push_back(message);
  }

  if (type & displaymask_) {
    std::string log_message;
    switch (type) {
    case kInfo:
      log_message = std::string(WHITE) + message + RESET;
      break;
    case kWarning:
      log_message = std::string(YELLOW) + message + RESET;
      break;
    case kError:
      log_message = std::string(RED) + message + RESET;
      break;
    case kConfirmation:
      log_message = std::string(BOLD_PURPLE) + message + RESET;
      break;
    default:
      log_message = message;
    }

    std::cout << log_message << std::endl;
  }
}
