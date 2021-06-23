#pragma once

#include "types.h"
#include <string>
#include <unordered_map>

//using Env = std::unordered_map<std::string, Result>;

class Env {
public:
  Env() {
      bindings["true"] = true;
      bindings["false"] = false;
  };

  Result* get(std::string key) {
    if (bindings.contains(key)) {
      return &bindings.at(key);
    } else {
      return nullptr;
    }
  }

  Result& operator[](std::string key) {
    return bindings[key];
  }

  void merge(Env other) {
    bindings.merge(other.bindings);
  }

  std::unordered_map<std::string, Result> bindings;
};