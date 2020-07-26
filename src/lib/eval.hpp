#pragma once

#include "env.hpp"
#include "expr.hpp"
#include "util.hpp"
#include <filesystem>
#include <string>

namespace afct {

Env Prelude();
Expr Eval(Expr const& expr, Env& env);
Expr Eval(std::string input, Env& env);
Expr Eval(std::string input);
Expr Eval(std::filesystem::path const& path, Env& env);
Expr Eval(std::filesystem::path const& path);

} // namespace afct
