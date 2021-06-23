#pragma once

#include <vector>

template <typename T> std::vector<T> rest(const std::vector<T> &v) {
  if (v.size() < 2) {
    return std::vector<T>();
  }
  std::vector<T> rest;
  rest.reserve(v.size() - 1);
  for (int i = 1; i < v.size(); ++i)
    rest.push_back(v[i]);
  return rest;
}