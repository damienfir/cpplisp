#include "builtins.h"

#include <iostream>

Number plus_fn(const std::vector<Result> &arguments) {
  Number n = 0;
  for (auto a : arguments) {
    n += std::get<Number>(a);
  }
  return n;
}

Number minus_fn(const std::vector<Result> &arguments) {
  Number n = std::get<Number>(arguments[0]);
  for (int i = 1; i < arguments.size(); ++i) {
    n -= std::get<Number>(arguments[i]);
  }
  return n;
}

Number divide_fn(const std::vector<Result> &arguments) {
  Number n = std::get<Number>(arguments[0]);
  for (int i = 1; i < arguments.size(); ++i) {
    n /= std::get<Number>(arguments[i]);
  }
  return n;
}

Number multiply_fn(const std::vector<Result> &arguments) {
  Number n = std::get<Number>(arguments[0]);
  for (int i = 1; i < arguments.size(); ++i) {
    n *= std::get<Number>(arguments[i]);
  }
  return n;
}

void check_one_args(const std::vector<Result> &arguments) {
  if (arguments.size() != 1) {
    throw std::runtime_error("'=' operator requires exactly 1 argument");
  }
}

void check_two_args(const std::vector<Result> &arguments) {
  if (arguments.size() != 2) {
    throw std::runtime_error("'=' operator requires exactly 2 arguments");
  }
}

bool equals_fn(const std::vector<Result> &arguments) {
  check_two_args(arguments);
  return std::get<Number>(arguments[0]) == std::get<Number>(arguments[1]);
}

bool less_than_fn(const std::vector<Result> &arguments) {
  check_two_args(arguments);
  return std::get<Number>(arguments[0]) < std::get<Number>(arguments[1]);
}

bool greater_than_fn(const std::vector<Result> &arguments) {
  check_two_args(arguments);
  return std::get<Number>(arguments[0]) > std::get<Number>(arguments[1]);
}

bool less_than_equals_fn(const std::vector<Result> &arguments) {
  return less_than_fn(arguments) || equals_fn(arguments);
}

bool greater_than_equals_fn(const std::vector<Result> &arguments) {
  return greater_than_fn(arguments) || equals_fn(arguments);
}

Number length_fn(const std::vector<Result> &arguments) {
  check_one_args(arguments);
  if (auto seq = std::get_if<List>(&arguments[0])) {
    return seq->list.size();
  } else {
    throw std::runtime_error("Can only get length of list");
  }
}

List cons_fn(std::vector<Result> arguments) {
  check_two_args(arguments);

  if (auto seq = std::get_if<List>(&arguments[1])) {
    seq->list.insert(seq->list.begin(), arguments[0]);
    return *seq;
  } else {
    throw std::runtime_error("Can only cons to list");
  }
}

List append_fn(std::vector<Result> arguments) {
  check_two_args(arguments);

  if (auto seq = std::get_if<List>(&arguments[1])) {
    seq->list.push_back(arguments[0]);
    return *seq;
  } else {
    throw std::runtime_error("Can only append to list");
  }
}

List concat_fn(const std::vector<Result> &arguments) {
  List list;
  for (auto arg : arguments) {
    if (auto l = std::get_if<List>(&arg)) {
      std::copy(l->list.begin(), l->list.end(), std::back_inserter(list.list));
    } else {
      throw std::runtime_error("Can only concat lists");
    }
  }
  return list;
}

Result get_fn(const std::vector<Result> &arguments) {
  if (auto seq = std::get_if<List>(&arguments[0])) {
    if (auto indice = std::get_if<Number>(&arguments[1])) {
      int i = (int)*indice;
      if (i <= seq->list.size()) {
        return seq->list[i];
      } else {
        throw std::runtime_error("Indice " + std::to_string(i) + " too high");
      }
    } else {
      throw std::runtime_error("'get' requires a number as indice");
    }
  } else {
    throw std::runtime_error("'get' requires a list");
  }
}

List list_fn(std::vector<Result> arguments) {
  return List(std::move(arguments));
}

Result first_fn(const std::vector<Result> &arguments) {
  if (auto seq = std::get_if<List>(&arguments[0])) {
    return seq->list[0];
  } else {
    throw std::runtime_error("Can only get first item from list");
  }
}

List rest_fn(std::vector<Result> arguments) {
  if (auto seq = std::get_if<List>(&arguments[0])) {
    seq->list.erase(seq->list.begin());
    return *seq;
  } else {
    throw std::runtime_error("Can only get the rest of a list");
  }
}

Nil println_fn(const std::vector<Result> &arguments) {
  for (int i = 0; i < arguments.size(); ++i) {
    std::cout << to_string(arguments[i]);
    if (i < arguments.size() - 1) {
      std::cout << " ";
    }
  }
  std::cout << std::endl;
  return Nil{};
}

bool not_fn(const std::vector<Result> &arguments) {
  if (arguments.size() != 1) {
    throw std::runtime_error("Expected one argument to 'not");
  }

  return !is_true(arguments[0]);
}

Result apply_op(const std::string &op, const std::vector<Result> &arguments) {
  if (op == "+") {
    return plus_fn(arguments);
  } else if (op == "-") {
    return minus_fn(arguments);
  } else if (op == "/") {
    return divide_fn(arguments);
  } else if (op == "*") {
    return multiply_fn(arguments);
  } else if (op == "=") {
    return equals_fn(arguments);
  } else if (op == ">") {
    return greater_than_fn(arguments);
  } else if (op == "<") {
    return less_than_fn(arguments);
  } else if (op == "<=") {
    return less_than_equals_fn(arguments);
  } else if (op == ">=") {
    return greater_than_equals_fn(arguments);
  } else if (op == "length") {
    return length_fn(arguments);
  } else if (op == "cons") {
    return cons_fn(arguments);
  } else if (op == "append") {
    return append_fn(arguments);
  } else if (op == "concat") {
    return concat_fn(arguments);
  } else if (op == "get") {
    return get_fn(arguments);
  } else if (op == "list") {
    return list_fn(arguments);
  } else if (op == "first") {
    return first_fn(arguments);
  } else if (op == "rest") {
    return rest_fn(arguments);
  } else if (op == "println") {
    return println_fn(arguments);
  } else if (op == "not") {
    return not_fn(arguments);
  } else {
    throw std::runtime_error("Unknown operation: " + op);
  }
}

struct TruthVisitor {
  bool operator()(Number n) { return n != 0; }

  bool operator()(Nil &) { return false; }

  bool operator()(Boolean b) { return b; }

  bool operator()(Symbol &) { return true; }

  bool operator()(List &) {
    throw std::runtime_error("Cannot get bool value from List");
  }

  bool operator()(Lambda &) {
    throw std::runtime_error("Cannot get bool value from Lambda");
  }

  bool operator()(const String &s) { return !s.empty(); }
};

bool is_true(Result res) { return std::visit(TruthVisitor{}, res); }