#include "prelude.hpp"

#include "function.hpp"
#include "parse.hpp"
#include "util.hpp"
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <optional>
#include <random>

namespace afct {

template<class Op>
Expr Reduce(
    std::string const& name,
    List const& args,
    Op op,
    std::optional<double> initial)
{
  if (args.size() < 2)
    AFCT_ARG_ERROR(args, "Expected 2+ args to " << name);

  double result = 0.0;
  if (initial.has_value())
  {
    result = initial.value();
  }
  else
  {
    auto const& first = args.front();
    if (first.is_numeric())
      result = first.get_numeric();
    else
      AFCT_ARG_ERROR(args, "Expected numeric args to " << name);
  }

  auto it = args.begin();
  if (!initial.has_value())
    std::advance(it, 1);
  while (it != args.end())
  {
    auto const& arg = *it;
    if (arg.is_numeric())
      result = op(result, arg.get_numeric());
    else
      AFCT_ARG_ERROR(args, "Expected numeric args to " << name);
    it++;
  }

  if (result == static_cast<uint64_t>(result))
    return Expr{result};
  else
    return Expr{result};
}

Expr Eq(List const& args)
{
  if (args.size() != 2)
    AFCT_ARG_ERROR(args, "Expected 2 args to =");

  return Expr{args[0] == args[1]};
}

Expr Add(List const& args)
{
  return Reduce("+", args, std::plus<double>(), 0.0);
}

Expr Sub(List const& args)
{
  return Reduce("-", args, std::minus<double>(), {});
}

Expr Mult(List const& args)
{
  return Reduce("*", args, std::multiplies<double>(), 1.0);
}

Expr Div(List const& args)
{
  return Reduce("/", args, std::divides<double>(), {});
}

Expr LessThan(List const& args)
{
  if (args.size() != 2)
    AFCT_ARG_ERROR(args, "Expected 2 args to <");

  double lhs, rhs;
  if (args[0].is_numeric() && args[1].is_numeric())
  {
    lhs = args[0].get_numeric();
    rhs = args[1].get_numeric();
  }
  else
    AFCT_ARG_ERROR(args, "Expected numeric args to <");

  return Expr{lhs < rhs};
}

Expr GreaterThan(List const& args)
{
  if (args.size() != 2)
    AFCT_ARG_ERROR(args, "Expected 2 args to >");

  double lhs, rhs;
  if (args[0].is_numeric() && args[1].is_numeric())
  {
    lhs = args[0].get_numeric();
    rhs = args[1].get_numeric();
  }
  else
    AFCT_ARG_ERROR(args, "Expected numeric args to >");

  return Expr{lhs > rhs};
}

Expr And(List const& args)
{
  if (args.size() < 2)
    AFCT_ARG_ERROR(args, "Expected 2+ args to and");

  for (auto const& arg : args)
  {
    if (!arg.truthy())
      return Expr{false};
  }
  return Expr{true};
}

Expr Or(List const& args)
{
  if (args.size() < 2)
    AFCT_ARG_ERROR(args, "Expected 2+ args to or");

  for (auto const& arg : args)
  {
    if (arg.truthy())
      return Expr{true};
  }
  return Expr{false};
}

Expr Not(List const& args)
{
  if (args.size() != 1)
    AFCT_ARG_ERROR(args, "Expected 1 arg to not");
  if (!args[0].is_bool())
    AFCT_ARG_ERROR(args, "Expected bool arg to not");

  return Expr{!args[0].get_bool()};
}

Expr ToList(List const& args)
{
  return Expr{args};
}

Expr ToTable(List const& args)
{
  if (args.size() % 2 != 0)
    AFCT_ARG_ERROR(args, "Expected even args to table");

  Table table;
  for (size_t i = 0; i < args.size(); i += 2)
    table[args[i]] = args[i + 1];
  return Expr{std::move(table)};
}

Expr Length(List const& args)
{
  if (args.size() != 1 || (!args[0].is_list() && !args[0].is_table()))
    AFCT_ARG_ERROR(args, "Expected 1 list or table arg to length");

  if (args[0].is_list())
    return Expr{static_cast<int64_t>(args[0].get_list()->size())};
  else
    return Expr{static_cast<int64_t>(args[0].get_table()->size())};
}

Expr Append(List const& args)
{
  if (args.size() < 2)
    AFCT_ARG_ERROR(args, "Expected 2+ args to append");
  if (!args[0].is_list())
    AFCT_ARG_ERROR(args, "Expected list arg to append");

  List result;
  for (auto const& first : *args[0].get_list())
    result.push_back(first);
  for (size_t i = 1; i < args.size(); i++)
    result.push_back(args[i]);
  return Expr{std::move(result)};
}

Expr Cons(List const& args)
{
  if (args.size() != 2)
    AFCT_ARG_ERROR(args, "Expected 2 args to cons");

  return Expr{List{args[0], args[1]}};
}

Expr Car(List const& args)
{
  if (args.size() != 1)
    AFCT_ARG_ERROR(args, "Expected 1 arg to car");
  if (!args[0].is_list())
    AFCT_ARG_ERROR(args, "Expected list arg to car");

  auto const& list = *args[0].get_list();
  if (list.empty())
    AFCT_ERROR("Zero-length list passed to car");
  return list[0];
}

Expr Cdr(List const& args)
{
  if (args.size() != 1)
    AFCT_ARG_ERROR(args, "Expected 1 arg to cdr");
  if (!args[0].is_list())
    AFCT_ARG_ERROR(args, "Expected list arg to cdr");

  List result;
  auto const& input = *args[0].get_list();
  for (size_t i = 1; i < input.size(); i++)
    result.push_back(input[i]);
  return Expr{std::move(result)};
}

Expr Cat(List const& args)
{
  if (args.empty())
    AFCT_ARG_ERROR(args, "Expected 1+ args to cat");

  std::ostringstream stream;
  for (auto const& arg : args)
  {
    if (!arg.is_string())
      AFCT_ARG_ERROR(args, "Expected string args to cat");

    stream << arg.get_string();
  }
  return Expr{String{stream.str()}};
}

Expr Get(List const& args)
{
  if (args.size() != 2 || !args[0].is_table())
    AFCT_ARG_ERROR(args, "Expected 2 args to get, table first");

  auto const& table = *args[0].get_table();
  auto const& key = args[1];
  auto it = table.find(key);
  if (it == table.end())
    return Expr{};
  else
    return it->second;
}

Expr SetBang(List const& args)
{
  if (args.size() != 3 || !args[0].is_table())
    AFCT_ARG_ERROR(args, "Expected 3 args to set!, table first");

  auto& table = *args[0].get_table();
  auto const& key = args[1];
  auto const& value = args[2];
  table[key] = value;
  return Expr{};
}

Expr Keys(List const& args)
{
  if (args.size() != 1 || !args[0].is_table())
    AFCT_ARG_ERROR(args, "Expected 1 table arg to keys");

  List result;
  for (auto const& pair : *args[0].get_table())
    result.push_back(pair.first);
  return Expr{std::move(result)};
}

Expr Values(List const& args)
{
  if (args.size() != 1 || !args[0].is_table())
    AFCT_ARG_ERROR(args, "Expected 1 table arg to values");

  List result;
  for (auto const& pair : *args[0].get_table())
    result.push_back(pair.second);
  return Expr{std::move(result)};
}

Expr Bool(List const& args)
{
  if (args.size() != 1)
    AFCT_ARG_ERROR(args, "Expected 1 arg to bool");

  return Expr{args[0].truthy()};
}

Expr Double(List const& args)
{
  if (args.size() != 1)
    AFCT_ARG_ERROR(args, "Expected 1 arg to double");

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
      AFCT_ERROR("Could not convert " << arg << " to double");
    }
  }

  AFCT_ERROR("Could not convert " << arg << " to double");
}

Expr Int(List const& args)
{
  if (args.size() != 1)
    AFCT_ARG_ERROR(args, "Expected 1 arg to int");

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
      AFCT_ERROR("Could not convert " << arg << " to int");
    }
  }

  AFCT_ERROR("Could not convert " << arg << " to int");
}

Expr ToString(List const& args)
{
  if (args.size() != 1)
    AFCT_ARG_ERROR(args, "Expected 1 arg to string");

  std::ostringstream stream;
  stream << args[0];
  return Expr{String{stream.str()}};
}

Expr Apply(List const& args)
{
  if (args.size() != 2)
    AFCT_ARG_ERROR(args, "Expected 2 args to apply");
  if (!args[0].is_function() || !args[1].is_list())
    AFCT_ARG_ERROR(args, "Expected function and list args to apply");

  auto function = args[0].get_function();
  return function->call(*args[1].get_list());
}

Expr Map(List const& args)
{
  if (args.size() != 2)
    AFCT_ARG_ERROR(args, "Expected 2 args to map");
  if (!args[0].is_function() || !args[1].is_list())
    AFCT_ARG_ERROR(args, "Expected function and list args to map");

  auto function = args[0].get_function();
  List result;
  for (auto const& element : *args[1].get_list())
    result.push_back(function->call({element}));
  return Expr{std::move(result)};
}

Expr Filter(List const& args)
{
  if (args.size() != 2)
    AFCT_ARG_ERROR(args, "Expected 2 args to filter");
  if (!args[0].is_function() || !args[1].is_list())
    AFCT_ARG_ERROR(args, "Expected function and list args to filter");

  auto function = args[0].get_function();
  List result;
  for (auto const& element : *args[1].get_list())
  {
    if (function->call({element}).truthy())
      result.push_back(element);
  }
  return Expr{std::move(result)};
}

Expr Print(List const& args)
{
  if (args.size() != 1)
    AFCT_ARG_ERROR(args, "Expected 1 arg to print");

  if (args[0].is_string())
    std::cout << args[0].get_string() << std::endl; // avoid ""
  else
    std::cout << args[0] << std::endl;
  return Expr{};
}

Expr GetEnv(List const& args)
{
  if (args.size() != 1 || !args[0].is_string())
    AFCT_ARG_ERROR(args, "Expected 1 string arg to getenv");

  char* result = std::getenv(args[0].get_string().c_str());
  if (!result)
    return Expr{String{""}};
  return Expr{String{result}};
}

Expr Rand(List const& args)
{
  if (args.size() != 2)
    AFCT_ARG_ERROR(args, "Expected 2 args to rand");
  if (!args[0].is_int() || !args[1].is_int())
    AFCT_ARG_ERROR(args, "Expected int args to rand");

  std::random_device device;
  std::mt19937 rng(device());
  std::uniform_int_distribution<std::mt19937::result_type> dist(
      args[0].get_int(), args[1].get_int());
  return Expr{static_cast<int64_t>(dist(rng))};
}

Env Prelude()
{
  auto prelude = Env();

  std::vector<std::pair<std::string, std::function<Expr(List const&)>>>
      symbol_and_function{
          {"=", Eq},          {"+", Add},           {"-", Sub},
          {"*", Mult},        {"/", Div},           {"<", LessThan},
          {">", GreaterThan}, {"and", And},         {"or", Or},
          {"not", Not},       {"list", ToList},     {"table", ToTable},
          {"length", Length}, {"append", Append},   {"cons", Cons},
          {"car", Car},       {"cdr", Cdr},         {"cat", Cat},
          {"get", Get},       {"set!", SetBang},    {"keys", Keys},
          {"values", Values}, {"bool", Bool},       {"double", Double},
          {"int", Int},       {"string", ToString}, {"apply", Apply},
          {"map", Map},       {"filter", Filter},   {"print", Print},
          {"getenv", GetEnv}, {"rand", Rand}};
  for (auto const& pair : symbol_and_function)
    prelude.set(pair.first, NativeFunctionToExpr(pair.first, pair.second));

  return prelude;
}

} // namespace afct
