#pragma once

#include "expr.hpp"
#include <list>

namespace afct {

std::list<std::string> Lex(std::string const& text);
Expr Parse(std::string text);

} // namespace afct
