#ifndef VARIABLE_H_
#define VARIABLE_H_

#include <iostream>
#include <string>
#include <unordered_map>
#include <typeinfo>
#include <typeindex>
#include <cstdint>

class Variable {
public:
  static std::unordered_map<std::string, Variable*> variables_;

  static void PrintVariables();
  static size_t GetSizeOfType(const std::string& typeName);

  ///////////
  // nameless variable
  template <typename T>
  Variable(T value) : variable_type_(typeid(T)) {
    variable_ = new T(value);
  }

  template <typename T>
  void Set(T value) {
    *static_cast<T*>(variable_) = value;
  }

  template <typename T>
  T as() {
    return *static_cast<T*>(variable_);
  }
  /////////

  template <typename T>
  static Variable* NewVariable(const std::string& name, T value = 0) {
    variables_[name] = new Variable(name, typeid(T));
    variables_[name]->variable_ = new T(value);
    return variables_[name];
  }

  template <typename T>
  static T ReadVariable(const std::string& name) {
    if (variables_.find(name) != variables_.end() && variables_[name]->variable_type_ == typeid(T)) {
      return *static_cast<T*>(variables_[name]->variable_);
    }
    throw std::runtime_error("Variable not found or type mismatch");
  }

  template <typename T>
  static void SetVariable(const std::string& name, T value) {
    if (variables_.find(name) != variables_.end() && variables_[name]->variable_type_ == typeid(T)) {
      *static_cast<T*>(variables_[name]->variable_) = value;
    }
    else {
      throw std::runtime_error("Variable not found or type mismatch");
    }
  }

  template <typename T>
  static Variable* GetVariable(const std::string& name) {
    if (variables_.find(name) != variables_.end()) {
      return variables_[name];
    }

    return NewVariable<T>(name);
    /*
    else {
      throw std::runtime_error("Variable not found or type mismatch");
    }
    */
  }

  void* variable_;
  std::type_index variable_type_;
private:
  Variable(const std::string& name, const std::type_info& type) : variable_(nullptr), variable_type_(type) { }
};

#endif
