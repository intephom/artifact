#include "util.hpp"

#include "eval.hpp"
#include "prelude.hpp"

namespace afct {

Expr EvalSimple(std::string input)
{
  auto env = Prelude();
  return Eval(std::move(input), env);
}

} // namespace afct
