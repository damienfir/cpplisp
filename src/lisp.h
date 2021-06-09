#pragma once

#include <string>
#include <utility>
#include <variant>
#include <vector>
#include <unordered_map>

#include "tokenizer.h"
#include "parser.h"
#include "types.h"

std::string stdlib();

using Env = std::unordered_map<std::string, Result>;

std::pair<Result, Env> eval(Expression expr, Env env);

std::pair<Result, Env> eval_with_env(std::string program, Env env);

Result eval_program(std::string program);
