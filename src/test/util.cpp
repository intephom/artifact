#include "eval.hpp"
#include "prelude.hpp"
#include "util.hpp"

namespace afct {

Expr EvalSimple(std::string input)
{
  auto env = Prelude();
  return Eval(std::move(input), env);
}

}
