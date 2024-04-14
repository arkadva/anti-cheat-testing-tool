#ifndef FUNCTIONHANDLERS_H_
#define FUNCTIONHANDLERS_H_

#include <Windows.h>
#include <string>
#include <vector>
#include <map>
#include <functional>

BOOL a_wrapper(std::vector<std::string> params);

extern std::map<std::string, std::function<BOOL(std::vector<std::string>)>> functionMap;

void InitializeFunctionMap();

BOOL HandleRequest(const std::string& request);

#endif
