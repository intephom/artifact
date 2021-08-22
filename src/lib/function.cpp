#include "function.hpp"

#include "eval.hpp"
#include <fmt/format.h>
#include <fmt/ostream.h>

namespace afct {

bool operator==(Lambda const& lhs, Lambda const& rhs)
{
  return lhs.params == rhs.params && lhs.body == rhs.body;
}

bool operator!=(Lambda const& lhs, Lambda const& rhs)
{
  return !(lhs == rhs);
}

std::ostream& operator<<(std::ostream& stream, Lambda const& lambda)
{
  return stream << "(lambda " << Expr{lambda.params} << " " << lambda.body
                << ")";
}

StdFunction::StdFunction(
    std::function<Expr(List&, std::shared_ptr<Env>)> function)
  : _function(std::move(function))
{}

Expr StdFunction::call(List& args, std::shared_ptr<Env> env)
{
  return _function(args, env);
}

Expr StdFunctionToExpr(
    std::string name, std::function<Expr(List&, std::shared_ptr<Env>)> function)
{
  auto native = std::make_shared<StdFunction>(std::move(function));
  return Expr{Builtin{std::move(name), std::move(native)}};
}

bool operator==(Builtin const& lhs, Builtin const& rhs)
{
  return lhs.name == rhs.name;
}

bool operator!=(Builtin const& lhs, Builtin const& rhs)
{
  return !(lhs == rhs);
}

std::ostream& operator<<(std::ostream& stream, Builtin const& builtin)
{
  return stream << builtin.name;
}

Expr Call(Expr const& expr, List& args, std::shared_ptr<Env> env)
{
  if (expr.is_builtin())
    return expr.get_builtin().function->call(args, env);

  AFCT_CHECK(expr.is_lambda(), "Expected function to call");
  auto const& lambda = expr.get_lambda();

  AFCT_CHECK(
      lambda.params.size() == args.size(),
      fmt::format(
          "Expected {} args to lambda but got {}",
          lambda.params.size(),
          args.size()));

  auto lambda_env = std::make_shared<Env>(env);
  for (size_t i = 0; i < lambda.params.size(); i++)
  {
    AFCT_CHECK(
        lambda.params[i].is_name(),
        fmt::format("Param {} is not a name", lambda.params[i]));
    lambda_env->set(lambda.params[i].get_name(), args[i]);
  }

  return Eval(lambda.body, lambda_env);
}

} // namespace afct
