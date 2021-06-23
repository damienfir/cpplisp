#pragma once

#include <string>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

#include "parser.h"
#include "tokenizer.h"
#include "types.h"

std::string stdlib();

Result eval_with_env(const std::string &program, Env &env);
Result eval_program(const std::string &program);
Result eval_program_with_stdlib(const std::string &program);
