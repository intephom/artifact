#include "prelude.hpp"

#include "eval.hpp"
#include "function.hpp"
#include "parse.hpp"
#include "util.hpp"
#include <cstdlib>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <functional>
#include <iostream>
#include <memory>
#include <optional>
#include <random>

namespace afct {

namespace {

template<class Op>
Expr Reduce(
    std::string const& name,
    List const& args,
    Op op,
    std::optional<double> initial)
{
  AFCT_ARG_CHECK(args.size() >= 2, fmt::format("Expected 2+ args to {}", name));

  double result = 0.0;
  if (initial.has_value())
  {
    result = initial.value();
  }
  else
  {
    auto const& first = args.front();
    AFCT_ARG_CHECK(
        first.is_numeric(), fmt::format("Expected numeric args to {}", name))
    result = first.get_numeric();
  }

  auto it = args.begin();
  if (!initial.has_value())
    std::advance(it, 1);
  while (it != args.end())
  {
    auto const& arg = *it;
    AFCT_ARG_CHECK(
        arg.is_numeric(), fmt::format("Expected numeric args to {}", name))
    result = op(result, arg.get_numeric());
    it++;
  }

  if (result == static_cast<uint64_t>(result))
    return Expr{result};
  else
    return Expr{result};
}

} // namespace

Expr Eq(List& args, std::shared_ptr<Env>)
{
  AFCT_ARG_CHECK(args.size() == 2, "Expected 2 args to =");
  return Expr{args[0] == args[1]};
}

Expr Add(List& args, std::shared_ptr<Env>)
{
  return Reduce("+", args, std::plus<double>(), 0.0);
}

Expr Sub(List& args, std::shared_ptr<Env>)
{
  return Reduce("-", args, std::minus<double>(), {});
}

Expr Mult(List& args, std::shared_ptr<Env>)
{
  return Reduce("*", args, std::multiplies<double>(), 1.0);
}

Expr Div(List& args, std::shared_ptr<Env>)
{
  return Reduce("/", args, std::divides<double>(), {});
}

Expr LessThan(List& args, std::shared_ptr<Env>)
{
  AFCT_ARG_CHECK(args.size() == 2, "Expected 2 args to <");

  double lhs, rhs;
  AFCT_ARG_CHECK(
      args[0].is_numeric() && args[1].is_numeric(),
      "Expected numeric args to <");
  lhs = args[0].get_numeric();
  rhs = args[1].get_numeric();
  return Expr{lhs < rhs};
}

Expr GreaterThan(List& args, std::shared_ptr<Env>)
{
  AFCT_ARG_CHECK(args.size() == 2, "Expected 2 args to >");

  double lhs, rhs;
  AFCT_ARG_CHECK(
      args[0].is_numeric() && args[1].is_numeric(),
      "Expected numeric args to >");
  lhs = args[0].get_numeric();
  rhs = args[1].get_numeric();
  return Expr{lhs > rhs};
}

Expr And(List& args, std::shared_ptr<Env>)
{
  AFCT_ARG_CHECK(args.size() >= 2, "Expected 2+ args to and");

  for (auto const& arg : args)
  {
    if (!arg.truthy())
      return Expr{false};
  }
  return Expr{true};
}

Expr Or(List& args, std::shared_ptr<Env>)
{
  AFCT_ARG_CHECK(args.size() >= 2, "Expected 2+ args to or");

  for (auto const& arg : args)
  {
    if (arg.truthy())
      return Expr{true};
  }
  return Expr{false};
}

Expr Not(List& args, std::shared_ptr<Env>)
{
  AFCT_ARG_CHECK(
      args.size() == 1 && args[0].is_bool(), "Expected 1 bool arg to not");

  return Expr{!args[0].get_bool()};
}

Expr Min(List& args, std::shared_ptr<Env>)
{
  AFCT_ARG_CHECK(
      args.size() == 1 && args[0].is_list(), "Expected list arg to min");

  Expr min;
  for (auto const& element : args[0].get_list())
  {
    AFCT_ARG_CHECK(element.is_numeric(), "Expected numeric list to min");

    if (min.is_null() || (element.get_numeric() < min.get_numeric()))
      min = element;
  }
  return min;
}

Expr Max(List& args, std::shared_ptr<Env>)
{
  AFCT_ARG_CHECK(
      args.size() == 1 && args[0].is_list(), "Expected list arg to max");

  Expr max;
  for (auto const& element : args[0].get_list())
  {
    AFCT_ARG_CHECK(element.is_numeric(), "Expected numeric list to max");

    if (max.is_null() || (element.get_numeric() > max.get_numeric()))
      max = element;
  }
  return max;
}

Expr ToList(List& args, std::shared_ptr<Env>)
{
  return Expr{args};
}

Expr ToTable(List& args, std::shared_ptr<Env>)
{
  AFCT_ARG_CHECK(args.size() % 2 == 0, "Expected even args to table");

  Table table;
  for (size_t i = 0; i < args.size(); i += 2)
    table[args[i]] = args[i + 1];
  return Expr{std::move(table)};
}

Expr Length(List& args, std::shared_ptr<Env>)
{
  AFCT_ARG_CHECK(
      args.size() == 1 && (args[0].is_list() || args[0].is_table()),
      "Expected 1 list or table arg to length");

  if (args[0].is_list())
    return Expr{static_cast<int64_t>(args[0].get_list().size())};
  else
    return Expr{static_cast<int64_t>(args[0].get_table().size())};
}

Expr Append(List& args, std::shared_ptr<Env>)
{
  AFCT_ARG_CHECK(
      args.size() >= 2 && args[0].is_list(),
      "Expected list and 1+ other args to append");

  List result;
  for (auto const& first : args[0].get_list())
    result.push_back(first);
  for (size_t i = 1; i < args.size(); i++)
    result.push_back(args[i]);
  return Expr{std::move(result)};
}

Expr Cons(List& args, std::shared_ptr<Env>)
{
  AFCT_ARG_CHECK(args.size() == 2, "Expected 2 args to cons");

  return Expr{List{args[0], args[1]}};
}

Expr Car(List& args, std::shared_ptr<Env>)
{
  AFCT_ARG_CHECK(
      args.size() == 1 && args[0].is_list(), "Expected 1 list arg to car");

  auto const& list = args[0].get_list();
  if (list.empty())
    AFCT_ERROR("Zero-length list passed to car");
  return list[0];
}

Expr Cdr(List& args, std::shared_ptr<Env>)
{
  AFCT_ARG_CHECK(
      args.size() == 1 && args[0].is_list(), "Expected 1 list arg to cdr");

  List result;
  auto const& input = args[0].get_list();
  for (size_t i = 1; i < input.size(); i++)
    result.push_back(input[i]);
  return Expr{std::move(result)};
}

Expr Cat(List& args, std::shared_ptr<Env>)
{
  AFCT_ARG_CHECK(!args.empty(), "Expected 1+ args to cat");

  std::ostringstream stream;
  for (auto const& arg : args)
  {
    AFCT_ARG_CHECK(arg.is_string(), "Expected string args to cat");
    stream << arg.get_string();
  }
  return Expr{String{stream.str()}};
}

Expr Get(List& args, std::shared_ptr<Env>)
{
  AFCT_ARG_CHECK(
      args.size() == 2 && args[0].is_table(),
      "Expected 2 args to get, table first");

  auto const& table = args[0].get_table();
  auto const& key = args[1];
  auto it = table.find(key);
  if (it == table.end())
    return Expr{};
  else
    return it->second;
}

Expr SetBang(List& args, std::shared_ptr<Env>)
{
  AFCT_ARG_CHECK(
      args.size() == 3 && args[0].is_table(),
      "Expected 3 args to set!, table first");

  auto& table = args[0].get_table();
  auto const& key = args[1];
  auto const& value = args[2];
  table[key] = value;
  return value;
}

Expr Keys(List& args, std::shared_ptr<Env>)
{
  AFCT_ARG_CHECK(
      args.size() == 1 && args[0].is_table(), "Expected 1 table arg to keys");

  List result;
  for (auto const& pair : args[0].get_table())
    result.push_back(pair.first);
  return Expr{std::move(result)};
}

Expr Values(List& args, std::shared_ptr<Env>)
{
  AFCT_ARG_CHECK(
      args.size() == 1 && args[0].is_table(), "Expected 1 table arg to values");

  List result;
  for (auto const& pair : args[0].get_table())
    result.push_back(pair.second);
  return Expr{std::move(result)};
}

Expr Bool(List& args, std::shared_ptr<Env>)
{
  AFCT_ARG_CHECK(args.size() == 1, "Expected 1 arg to bool");

  return Expr{args[0].truthy()};
}

Expr Double(List& args, std::shared_ptr<Env>)
{
  AFCT_ARG_CHECK(args.size() == 1, "Expected 1 arg to double");

  auto const& arg = args[0];

  if (arg.is_double())
    return arg;
  else if (arg.is_int())
    return Expr{arg.get_int()};
  else if (arg.is_string())
  {
    try
    {
      return Expr{std::stod(arg.get_string())};
    }
    catch (std::exception const&)
    {
      AFCT_ERROR(fmt::format("Could not convert {} to double", arg))
    }
  }

  AFCT_ERROR(fmt::format("Could not convert {} to double", arg));
}

Expr Int(List& args, std::shared_ptr<Env>)
{
  AFCT_ARG_CHECK(args.size() == 1, "Expected 1 arg to int");

  auto const& arg = args[0];

  if (arg.is_int())
    return arg;
  else if (arg.is_double())
    return Expr{static_cast<int64_t>(arg.get_double())};
  else if (arg.is_string())
  {
    try
    {
      return Expr{static_cast<int64_t>(std::stoll(arg.get_string()))};
    }
    catch (std::exception const&)
    {
      AFCT_ERROR(fmt::format("Could not convert {} to int", arg))
    }
  }

  AFCT_ERROR(fmt::format("Could not convert {} to int", arg))
}

Expr ToString(List& args, std::shared_ptr<Env>)
{
  AFCT_ARG_CHECK(args.size() == 1, "Expected 1 arg to string");

  return Expr{String{fmt::format("{}", args[0])}};
}

Expr Apply(List& args, std::shared_ptr<Env> env)
{
  AFCT_ARG_CHECK(
      args.size() == 2 && args[0].is_function() && args[1].is_list(),
      "Expected function and list args to apply");

  return Call(args[0], args[1].get_list(), env);
}

Expr Map(List& args, std::shared_ptr<Env> env)
{
  AFCT_ARG_CHECK(
      args.size() == 2 && args[0].is_function() && args[1].is_list(),
      "Expected function and list args to map");

  List result;
  for (auto const& element : args[1].get_list())
  {
    std::vector<Expr> arg{element};
    result.push_back(Call(args[0], arg, env));
  }
  return Expr{std::move(result)};
}

Expr Filter(List& args, std::shared_ptr<Env> env)
{
  AFCT_ARG_CHECK(
      args.size() == 2 && args[0].is_function() && args[1].is_list(),
      "Expected function and list args to filter");

  List result;
  for (auto const& element : args[1].get_list())
  {
    std::vector<Expr> arg{element};
    if (Call(args[0], arg, env).truthy())
      result.push_back(element);
  }
  return Expr{std::move(result)};
}

Expr Print(List& args, std::shared_ptr<Env>)
{
  AFCT_ARG_CHECK(args.size() == 1, "Expected 1 arg to print");

  if (args[0].is_string())
    std::cout << args[0].get_string() << std::endl; // avoid ""
  else
    std::cout << args[0] << std::endl;
  return Expr{};
}

Expr GetEnv(List& args, std::shared_ptr<Env>)
{
  AFCT_ARG_CHECK(
      args.size() == 1 && args[0].is_string(),
      "Expected 1 string arg to getenv");

  char* result = std::getenv(args[0].get_string().c_str());
  if (!result)
    return Expr{String{""}};
  return Expr{String{result}};
}

Expr Rand(List& args, std::shared_ptr<Env>)
{
  AFCT_ARG_CHECK(
      args.size() == 2 && args[0].is_int() && args[1].is_int(),
      "Expected 2 int args to rand");

  std::random_device device;
  std::mt19937 rng(device());
  std::uniform_int_distribution<std::mt19937::result_type> dist(
      args[0].get_int(), args[1].get_int());
  return Expr{static_cast<int64_t>(dist(rng))};
}

std::shared_ptr<Env> Prelude()
{
  auto prelude = std::make_shared<Env>();

  std::vector<
      std::pair<std::string, std::function<Expr(List&, std::shared_ptr<Env>)>>>
      symbol_and_function{{"=", Eq},          {"+", Add},
                          {"-", Sub},         {"*", Mult},
                          {"/", Div},         {"<", LessThan},
                          {">", GreaterThan}, {"and", And},
                          {"or", Or},         {"not", Not},
                          {"min", Min},       {"max", Max},
                          {"list", ToList},   {"table", ToTable},
                          {"length", Length}, {"append", Append},
                          {"cons", Cons},     {"car", Car},
                          {"cdr", Cdr},       {"cat", Cat},
                          {"get", Get},       {"set!", SetBang},
                          {"keys", Keys},     {"values", Values},
                          {"bool", Bool},     {"double", Double},
                          {"int", Int},       {"string", ToString},
                          {"apply", Apply},   {"map", Map},
                          {"filter", Filter}, {"print", Print},
                          {"getenv", GetEnv}, {"rand", Rand}};
  for (auto& pair : symbol_and_function)
    prelude->set(
        pair.first, StdFunctionToExpr(pair.first, std::move(pair.second)));

  return prelude;
}

} // namespace afct
