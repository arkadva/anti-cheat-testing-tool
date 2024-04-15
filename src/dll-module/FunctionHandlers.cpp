#include <Windows.h>
#include <sstream>
#include <iostream>
#include "FunctionHandlers.h"

std::map<std::string, std::function<BOOL(std::vector<std::string>)>> functionMap;

void InitializeFunctionMap() {
  functionMap["PageGuardHook"] = PageGuardHook_wrapper;
}

BOOL HandleRequest(const std::string& request) {
  size_t pos = request.find('|');

  if (pos == std::string::npos) {
    return FALSE;
  }

  std::string command = request.substr(0, pos);
  std::string paramStr = request.substr(pos + 1);

  std::vector<std::string> paramTokens;
  std::istringstream iss(paramStr);
  std::string token;

  while (std::getline(iss, token, ';')) {
    paramTokens.push_back(token);
  }

  BOOL result = FALSE;
  if (functionMap.find(command) != functionMap.end()) {
    result = functionMap[command](paramTokens);
  }

  return result;
}

BOOL PageGuardHook_wrapper(std::vector<std::string> params) {
  void* address = reinterpret_cast<void*>(std::stoull(params[0], nullptr, 16));
  PageGuardHook(address);

  return TRUE;
}
