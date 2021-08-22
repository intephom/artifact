#pragma once

#include "env.hpp"
#include "expr.hpp"
#include "prelude.hpp"
#include "util.hpp"
#include <filesystem>
#include <memory>
#include <string>

namespace afct {

Expr Eval(Expr expr, std::shared_ptr<Env> env);
Expr Eval(std::string input, std::shared_ptr<Env> env);
Expr Eval(std::filesystem::path const& path, std::shared_ptr<Env> env);

} // namespace afct
