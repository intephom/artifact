#include "util.hpp"

#include "lib/eval.hpp"
#include "lib/prelude.hpp"

namespace afct {

Expr EvalSimple(std::string input)
{
  auto env = Prelude();
  return Eval(std::move(input), env);
}

} // namespace afct
