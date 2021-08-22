#include "eval.hpp"

#include "function.hpp"
#include "parse.hpp"
#include "prelude.hpp"
#include "util.hpp"
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <fstream>
#include <iostream>

#define AFCT_EVAL_CHECK(__predicate, __message) \
  AFCT_CHECK(__predicate, fmt::format("{} but got {}", __message, expr));

namespace afct {

Expr Eval(Expr expr, std::shared_ptr<Env> outer_env)
{
  std::shared_ptr<Env> inner_env;

  while (true)
  {
    auto env = inner_env ? inner_env : outer_env;

    if (expr.is_null() || expr.is_bool() || expr.is_double() || expr.is_int() ||
        expr.is_string())
    {
      return expr;
    }
    else if (expr.is_name())
    {
      auto const& key = expr.get_name();
      auto result = env->find(key);
      AFCT_CHECK(
          result.has_value(), fmt::format("Undefined identifier {}", key));
      return result.value();
    }
    else if (expr.is_table())
    {
      Table table;
      for (auto const& pair : expr.get_table())
        table[Eval(pair.first, env)] = Eval(pair.second, env);
      return Expr{table};
    }

    AFCT_EVAL_CHECK(expr.is_list(), "Expected list");
    auto const& list = expr.get_list();

    if (list.empty())
      return expr;

    AFCT_EVAL_CHECK(list.front().is_name(), "Expected name at start of list");
    auto const& name = list.front().get_name();

    if (name == "quote")
    {
      AFCT_EVAL_CHECK(list.size() == 2, "Expected 1 arg to quote");

      return list[1];
    }
    else if (name == "if")
    {
      AFCT_EVAL_CHECK(list.size() == 4, "Expected 3 args to if");

      auto const& cond = list[1];
      auto const& conseq = list[2];
      auto const& alt = list[3];
      if (Eval(cond, env).truthy())
        expr = conseq;
      else
        expr = alt;
      continue;
    }
    else if (name == "define")
    {
      AFCT_EVAL_CHECK(list.size() == 3, "Expected 2 args to define");

      auto const& key = list[1];
      AFCT_EVAL_CHECK(key.is_name(), "Expected name key to define");
      auto value = Eval(list[2], env);
      env->set(key.get_name(), value);
      return value;
    }
    else if (name == "lambda")
    {
      AFCT_EVAL_CHECK(list.size() == 3, "Expected 2 args to lambda");

      auto const& args = list[1];
      auto const& body = list[2];
      AFCT_EVAL_CHECK(args.is_list(), "Expected list args in lambda");
      return Expr{Lambda{args.get_list(), body, env}};
    }
    else if (name == "begin")
    {
      AFCT_EVAL_CHECK(list.size() > 1, "Expected 1+ args to begin");

      for (size_t i = 1; i < list.size() - 1; i++)
        Eval(list[i], env);

      expr = list.back();
      continue;
    }
    else
    {
      auto function = Eval(list.front(), env);

      if (function.is_lambda())
      {
        auto const& lambda = function.get_lambda();

        auto n_args = list.size() - 1;
        AFCT_CHECK(
            lambda.params.size() == n_args,
            fmt::format(
                "Expected {} args to lambda but got {}",
                lambda.params.size(),
                n_args));

        auto lambda_env = std::make_shared<Env>(Env(lambda.env));
        for (size_t i = 0; i < n_args; i++)
        {
          AFCT_CHECK(
              lambda.params[i].is_name(),
              fmt::format("Param {} is not a name", lambda.params[i]));
          lambda_env->set(lambda.params[i].get_name(), Eval(list[i + 1], env));
        }

        expr = lambda.body;
        inner_env = lambda_env;
        continue;
      }
      else if (function.is_builtin())
      {
        List args;
        for (size_t i = 1; i < list.size(); i++)
          args.push_back(Eval(list[i], env));

        return function.get_builtin().function->call(args, env);
      }

      AFCT_ERROR(fmt::format("Expected function but got {}", expr));
    }
  }
}

Expr Eval(std::string input, std::shared_ptr<Env> env)
{
  return Eval(Parse(std::move(input)), env);
}

Expr Eval(std::filesystem::path const& path, std::shared_ptr<Env> env)
{
  auto file = std::ifstream(path);
  AFCT_CHECK(file.good(), fmt::format("Failed to open {}", path));

  std::string input(
      (std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

  return Eval(std::move(input), env);
}

} // namespace afct
