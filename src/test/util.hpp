#pragma once

#include "lib/expr.hpp"

namespace afct {

// env gets destroyed so use when output does not depend on it
Expr EvalSimple(std::string input);

} // namespace afct
