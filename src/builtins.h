#pragma once

#include "ast.h"

Result apply_op(const std::string &op, const std::vector<Result> &arguments);
bool is_true(Result res);