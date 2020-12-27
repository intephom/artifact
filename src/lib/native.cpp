#include "native.hpp"
#include "parse.hpp"
#include "util.hpp"
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <optional>
#include <random>

#define AFCT_ARG_ERROR(message) \
  do \
  { \
    std::ostringstream stream; \
    stream << message << ", got " << afct::Expr::FromList(std::make_shared<afct::List>(args)); \
    throw std::runtime_error(stream.str()); \
  } while (false);

namespace afct {

template<class Op>
Expr Reduce(std::string const& name, List const& args, Op op, std::optional<double> initial)
{
  if (args.size() < 2)
    AFCT_ARG_ERROR("Expected 2+ args to " << name);

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
      AFCT_ARG_ERROR("Expected numeric args to " << name);
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
      AFCT_ARG_ERROR("Expected numeric args to " << name);
    it++;
  }

  if (result == static_cast<uint64_t>(result))
    return Expr::FromInt(result);
  else
    return Expr::FromDouble(result);
}

Expr Eq(List const& args)
{
  if (args.size() != 2)
    AFCT_ARG_ERROR("Expected 2 args to =");

  return Expr::FromBool(args[0] == args[1]);
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
    AFCT_ARG_ERROR("Expected 2 args to <");

  double lhs, rhs;
  if (args[0].is_numeric() && args[1].is_numeric())
  {
    lhs = args[0].get_numeric();
    rhs = args[1].get_numeric();
  }
  else
    AFCT_ARG_ERROR("Expected numeric args to <");

  return Expr::FromBool(lhs < rhs);
}

Expr GreaterThan(List const& args)
{
  if (args.size() != 2)
    AFCT_ARG_ERROR("Expected 2 args to >");

  double lhs, rhs;
  if (args[0].is_numeric() && args[1].is_numeric())
  {
    lhs = args[0].get_numeric();
    rhs = args[1].get_numeric();
  }
  else
    AFCT_ARG_ERROR("Expected numeric args to >");

  return Expr::FromBool(lhs > rhs);
}

Expr And(List const& args)
{
  if (args.size() < 2)
    AFCT_ARG_ERROR("Expected 2+ args to and");

  for (auto const& arg : args)
  {
    if (!arg.truthy())
      return Expr::FromBool(false);
  }
  return Expr::FromBool(true);
}

Expr Or(List const& args)
{
  if (args.size() < 2)
    AFCT_ARG_ERROR("Expected 2+ args to or");

  for (auto const& arg : args)
  {
    if (arg.truthy())
      return Expr::FromBool(true);
  }
  return Expr::FromBool(false);
}

Expr Not(List const& args)
{
  if (args.size() != 1)
    AFCT_ARG_ERROR("Expected 1 arg to not");
  if (!args[0].is_bool())
    AFCT_ARG_ERROR("Expected bool arg to not");

  return Expr::FromBool(!args[0].get_bool());
}

Expr ToList(List const& args)
{
  return Expr::FromList(std::make_shared<List>(args));
}

Expr ToTable(List const& args)
{
  auto table = std::make_shared<Table>();
  if (args.size() % 2 != 0)
    AFCT_ARG_ERROR("Expected even args to table");

  for (size_t i = 0; i < args.size(); i += 2)
    table->emplace(args[i], args[i + 1]);
  return Expr::FromTable(std::move(table));
}

Expr Length(List const& args)
{
  if (args.size() != 1 || !args[0].is_list())
    AFCT_ARG_ERROR("Expected 1 list arg to length");

  return Expr::FromInt(args[0].get_list()->size());
}

Expr Append(List const& args)
{
  if (args.size() < 2)
    AFCT_ARG_ERROR("Expected 2+ args to append");
  if (!args[0].is_list())
    AFCT_ARG_ERROR("Expected list arg to append");

  auto result = std::make_shared<List>();
  for (auto const& first : *args[0].get_list())
    result->push_back(first);
  for (size_t i = 1; i < args.size(); i++)
    result->push_back(args[i]);
  return Expr::FromList(std::move(result));
}

Expr Cons(List const& args)
{
  if (args.size() != 2)
    AFCT_ARG_ERROR("Expected 2 args to cons");

  auto result = List{args[0], args[1]};
  return Expr::FromList(std::make_shared<List>(std::move(result)));
}

Expr Car(List const& args)
{
  if (args.size() != 1)
    AFCT_ARG_ERROR("Expected 1 arg to car");
  if (!args[0].is_list())
    AFCT_ARG_ERROR("Expected list arg to car");

  auto const& list = *args[0].get_list();
  if (list.empty())
    AFCT_ARG_ERROR("Zero-length list passed to car");

  return list[0];
}

Expr Cdr(List const& args)
{
  if (args.size() != 1)
    AFCT_ARG_ERROR("Expected 1 arg to cdr");
  if (!args[0].is_list())
    AFCT_ARG_ERROR("Expected list arg to cdr");

  auto result = std::make_shared<List>();
  auto const& input = *args[0].get_list();
  for (size_t i = 1; i < input.size(); i++)
    result->push_back(input[i]);
  return Expr::FromList(std::move(result));
}

Expr Cat(List const& args)
{
  if (args.empty())
    AFCT_ARG_ERROR("Expected 1+ args to cat");

  std::ostringstream stream;
  for (auto const& arg : args)
  {
    if (!arg.is_string())
      AFCT_ARG_ERROR("Expected string args to cat");

    stream << arg.get_string();
  }
  return Expr::FromString(stream.str());
}

Expr Get(List const& args)
{
  if (args.size() != 2 || !args[0].is_table())
    AFCT_ARG_ERROR("Expected 2 args to get, table first");

  auto const& table = *args[0].get_table();
  auto const& key = args[1];
  auto it = table.find(key);
  if (it == table.end())
    return Expr::FromNull();
  else
    return it->second;
}

Expr SetBang(List const& args)
{
  if (args.size() != 3 || !args[0].is_table())
    AFCT_ARG_ERROR("Expected 3 args to set!, table first");

  auto& table = *args[0].get_table();
  auto const& key = args[1];
  auto const& value = args[2];
  table[key] = value;
  return Expr::FromNull();
}

Expr Bool(List const& args)
{
  if (args.size() != 1)
    AFCT_ARG_ERROR("Expected 1 arg to bool");

  return Expr::FromBool(args[0].truthy());
}

Expr Double(List const& args)
{
  if (args.size() != 1)
    AFCT_ARG_ERROR("Expected 1 arg to double");

  auto const& arg = args[0];

  if (arg.is_double())
    return arg;
  else if (arg.is_int())
    return Expr::FromDouble(arg.get_int());
  else if (arg.is_string())
  {
    try
    {
      return Expr::FromDouble(std::stod(arg.get_string()));
    }
    catch (std::exception const&)
    {
      AFCT_ARG_ERROR("Could not convert " << arg << " to double");
    }
  }

  AFCT_ARG_ERROR("Could not convert " << arg << " to double");
}

Expr Int(List const& args)
{
  if (args.size() != 1)
    AFCT_ARG_ERROR("Expected 1 arg to int");

  auto const& arg = args[0];

  if (arg.is_int())
    return arg;
  else if (arg.is_double())
    return Expr::FromInt(arg.get_double());
  else if (arg.is_string())
  {
    try
    {
      return Expr::FromInt(std::stoi(arg.get_string()));
    }
    catch (std::exception const&)
    {
      AFCT_ARG_ERROR("Could not convert " << arg << " to int");
    }
  }

  AFCT_ARG_ERROR("Could not convert " << arg << " to int");
}

Expr String(List const& args)
{
  if (args.size() != 1)
    AFCT_ARG_ERROR("Expected 1 arg to string");

  std::ostringstream stream;
  stream << args[0];
  return Expr::FromString(stream.str());
}

Expr Apply(List const& args)
{
  if (args.size() != 2)
    AFCT_ARG_ERROR("Expected 2 args to apply");
  if (!args[0].is_function() || !args[1].is_list())
    AFCT_ARG_ERROR("Expected function and list args to apply");

  auto function = args[0].get_function();
  return function->call(*args[1].get_list());
}

Expr Map(List const& args)
{
  if (args.size() != 2)
    AFCT_ARG_ERROR("Expected 2 args to map");
  if (!args[0].is_function() || !args[1].is_list())
    AFCT_ARG_ERROR("Expected function and list args to map");

  auto function = args[0].get_function();
  auto result = std::make_shared<List>();
  for (auto const& element : *args[1].get_list())
    result->push_back(function->call({element}));
  return Expr::FromList(std::move(result));
}

Expr Filter(List const& args)
{
  if (args.size() != 2)
    AFCT_ARG_ERROR("Expected 2 args to filter");
  if (!args[0].is_function() || !args[1].is_list())
    AFCT_ARG_ERROR("Expected function and list args to filter");

  auto function = args[0].get_function();
  auto result = std::make_shared<List>();
  for (auto const& element : *args[1].get_list())
  {
    if (function->call({element}).truthy())
      result->push_back(element);
  }
  return Expr::FromList(std::move(result));
}

Expr Print(List const& args)
{
  if (args.size() != 1)
    AFCT_ARG_ERROR("Expected 1 arg to print");

  std::cout << args[0] << std::endl;
  return Expr::FromNull();
}

Expr GetEnv(List const& args)
{
  if (args.size() != 1 || !args[0].is_string())
    AFCT_ARG_ERROR("Expected 1 string arg to getenv");

  char* result = std::getenv(args[0].get_string().c_str());
  if (!result)
    return Expr::FromString("");
  return Expr::FromString(result);
}

Expr Rand(List const& args)
{
  if (args.size() != 2)
    AFCT_ARG_ERROR("Expected 2 args to rand");
  if (!args[0].is_int() || !args[1].is_int())
    AFCT_ARG_ERROR("Expected int args to rand");

  std::random_device device;
  std::mt19937 rng(device());
  std::uniform_int_distribution<std::mt19937::result_type> dist(args[0].get_int(), args[1].get_int());
  return Expr::FromInt(dist(rng));
}

Expr NativeFuncToExpr(std::string name, std::function<Expr(List const&)> f)
{
  return Expr::FromFunction(std::make_shared<NativeFunction>(std::move(name), f));
}

}
