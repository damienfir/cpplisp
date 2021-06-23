#include "env.h"

//Env make_env(Expression::List vars, Env env) {
//  Env new_env;
//  for (int i = 0; i < vars.size(); i += 2) {
//    auto key = vars[i].get_symbol();
//    auto val = eval(vars[i + 1], env).first;
//    new_env[key] = val;
//  }
//  return new_env;
//}

//Env merge(Env a, Env b) {
//  // modifies both a and b but it's ok because we get copies as arguments
//  a.merge(b);
//  return a;
//}