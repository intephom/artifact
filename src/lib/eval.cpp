#include "eval.hpp"

#include "function.hpp"
#include "parse.hpp"
#include "prelude.hpp"
#include <fstream>

#define AFCT_EVAL_ERROR(message) \
  do \
  { \
    std::ostringstream stream; \
    stream << message << ", got " << expr; \
    throw std::runtime_error(stream.str()); \
  } while (false);

namespace afct {

Expr Eval(Expr const& expr, Env& env)
{
  if (expr.is_null() || expr.is_bool() || expr.is_double() || expr.is_int() ||
      expr.is_string() || expr.is_table())
  {
    return expr;
  }
  else if (expr.is_name())
  {
    return env.find(expr.get_name());
  }
  else if (!expr.is_list())
  {
    AFCT_EVAL_ERROR("Expected list");
  }

  auto const& list = *expr.get_list();

  if (list.empty())
    return expr;

  if (!list.front().is_name())
  {
    AFCT_EVAL_ERROR("Expected name at start of list");
  }
  auto const& name = list.front().get_name();

  if (name == "quote")
  {
    if (list.size() != 2)
      AFCT_EVAL_ERROR("Expected 1 arg to quote");

    return list[1];
  }
  else if (name == "if")
  {
    if (list.size() != 4)
      AFCT_EVAL_ERROR("Expected 3 args to if");

    auto const& cond = list[1];
    auto const& conseq = list[2];
    auto const& alt = list[3];
    return Eval(cond, env).truthy() ? Eval(conseq, env) : Eval(alt, env);
  }
  else if (name == "define")
  {
    if (list.size() != 3)
      AFCT_EVAL_ERROR("Expected 2 args to define");

    auto const& key = list[1];
    auto const& value = list[2];
    if (!key.is_name())
      AFCT_EVAL_ERROR("Expected name key to define");
    env.set(key.get_name(), Eval(value, env));
    return Expr::FromNull();
  }
  else if (name == "lambda")
  {
    if (list.size() != 3)
      AFCT_EVAL_ERROR("Expected 2 args to lambda");

    auto const& args = list[1];
    auto const& body = list[2];
    if (!args.is_list())
      AFCT_EVAL_ERROR("Expected list args in lambda");
    return Expr::FromFunction(
        std::make_shared<Function>("lambda", *args.get_list(), body, &env));
  }
  else if (name == "begin")
  {
    for (size_t i = 1; i < list.size(); i++)
    {
      if (i != list.size() - 1)
        Eval(list[i], env);
      else
        return Eval(list[i], env);
    }
    return Expr::FromNull();
  }
  else
  {
    auto func = Eval(list.front(), env);
    if (!func.is_function())
      AFCT_EVAL_ERROR("Expected function");
    List args;
    for (size_t i = 1; i < list.size(); i++)
      args.push_back(Eval(list[i], env));
    return func.get_function()->call(args);
  }
}

Expr Eval(std::string input, Env& env)
{
  auto expression = Parse(std::move(input));
  return Eval(expression, env);
}

Expr Eval(std::filesystem::path const& path, Env& env)
{
  auto file = std::ifstream(path);
  if (!file.good())
    AFCT_ERROR("Failed to open " << path);

  std::string input(
      (std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

  return Eval(input, env);
}

} // namespace afct
