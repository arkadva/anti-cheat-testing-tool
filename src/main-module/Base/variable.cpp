#include "variable.h"
#include <cstdint>
#include <stdexcept>

std::unordered_map<std::string, Variable*> Variable::variables_;

void Variable::PrintVariables() {
  std::cout << "Print start!!!" << std::endl;
  std::cout << variables_.size() << std::endl;

  for (const auto& var : variables_) {
    std::cout << var.first << ": ";

    if (var.second->variable_type_ == typeid(int8_t)) {
      std::cout << *static_cast<int8_t*>(var.second->variable_);
    }
    else if (var.second->variable_type_ == typeid(int16_t)) {
      std::cout << *static_cast<int16_t*>(var.second->variable_);
    }
    else if (var.second->variable_type_ == typeid(int32_t)) {
      std::cout << *static_cast<int32_t*>(var.second->variable_);
    }
    else if (var.second->variable_type_ == typeid(int64_t)) {
      std::cout << *static_cast<int64_t*>(var.second->variable_);
    }
    else if (var.second->variable_type_ == typeid(uint8_t)) {
      std::cout << *static_cast<uint8_t*>(var.second->variable_);
    }
    else if (var.second->variable_type_ == typeid(uint16_t)) {
      std::cout << *static_cast<uint16_t*>(var.second->variable_);
    }
    else if (var.second->variable_type_ == typeid(uint32_t)) {
      std::cout << *static_cast<uint32_t*>(var.second->variable_);
    }
    else if (var.second->variable_type_ == typeid(uint64_t)) {
      std::cout << *static_cast<uint64_t*>(var.second->variable_);
    }
    else {
      std::cout << "Unknown type";
    }
    std::cout << std::endl;
  }
}

size_t Variable::GetSizeOfType(const std::string& typeName) {
  static const std::unordered_map<std::string, size_t(*)(void)> typeMap = {
      {"uint8_t", []() { return sizeof(uint8_t); }},
      {"int8_t", []() { return sizeof(int8_t); }},
      {"uint16_t", []() { return sizeof(uint16_t); }},
      {"int16_t", []() { return sizeof(int16_t); }},
      {"uint32_t", []() { return sizeof(uint32_t); }},
      {"int32_t", []() { return sizeof(int32_t); }},
      {"uint64_t", []() { return sizeof(uint64_t); }},
      {"int64_t", []() { return sizeof(int64_t); }},
      {"float", []() { return sizeof(float); }},
      {"double", []() { return sizeof(double); }},
      {"char", []() { return sizeof(char); }},
  };

  auto it = typeMap.find(typeName);
  if (it != typeMap.end()) {
    return it->second();
  }
  else {
    throw std::invalid_argument("Unknown type name: " + typeName);
  }
}
