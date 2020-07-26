#pragma once

#include "expr.hpp"
#include <list>

namespace afct {

std::list<std::string> Lex(std::string const& text);
Expr Parse(std::list<std::string>& tokens);
Expr Parse(std::string const& text);

} // namespace afct
