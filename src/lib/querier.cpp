#include "parse.hpp"
#include "querier.hpp"

namespace afct {

Querier::Querier(Expr expr)
  : _root(expr)
{}

bool Querier::get_list(
  std::string const& path,
  std::vector<Expr>& result) const
{
  auto elements = Split("/", path);
  Expr expr;
  if (!find(elements, expr))
    return false;

  if (expr.is_list())
  {
    if (IsQuote(expr))
    {
      auto const& unquoted = Unquote(expr);
      if (unquoted.is_list())
      {
        result = *unquoted.get_list();
        return true;
      }
    }

    result = *expr.get_list();
    return true;
  }
  else if (expr.is_table())
  {
    for(auto const& pair : *expr.get_table())
    {
      std::vector<Expr> list({pair.first, pair.second});
      result.push_back(Expr::FromList(std::make_shared<List>(std::move(list))));
    }
    return true;
  }

  return false;
}

std::vector<Expr> Querier::get_list(std::string const& path) const
{
  std::vector<Expr> result;
  if (!get_list(path, result))
    AFCT_ERROR("List for " << path << " not found");
  return result;
}

template<>
bool Querier::get<bool>(Expr const& expr, bool& result) const
{
  if (!expr.is_bool())
    return false;
  result = expr.get_bool();
  return true;
}

template<>
bool Querier::get<double>(Expr const& expr, double& result) const
{
  if (expr.is_double())
  {
    result = expr.get_double();
    return true;
  }
  else if (expr.is_int())
  {
    result = expr.get_int();
    return true;
  }
  else
    return false;
}

template<>
bool Querier::get<int8_t>(Expr const& expr, int8_t& result) const
{
  return get_int<int8_t>(expr, result);
}

template<>
bool Querier::get<int16_t>(Expr const& expr, int16_t& result) const
{
  return get_int<int16_t>(expr, result);
}

template<>
bool Querier::get<int32_t>(Expr const& expr, int32_t& result) const
{
  return get_int<int32_t>(expr, result);
}

template<>
bool Querier::get<int64_t>(Expr const& expr, int64_t& result) const
{
  return get_int<int64_t>(expr, result);
}

template<>
bool Querier::get<uint8_t>(Expr const& expr, uint8_t& result) const
{
  return get_int<uint8_t>(expr, result);
}

template<>
bool Querier::get<uint16_t>(Expr const& expr, uint16_t& result) const
{
  return get_int<uint16_t>(expr, result);
}

template<>
bool Querier::get<uint32_t>(Expr const& expr, uint32_t& result) const
{
  return get_int<uint32_t>(expr, result);
}

template<>
bool Querier::get<uint64_t>(Expr const& expr, uint64_t& result) const
{
  return get_int<uint64_t>(expr, result);
}

template<>
bool Querier::get<std::string>(Expr const& expr, std::string& result) const
{
  if (expr.is_string())
  {
    result = expr.get_string();
    return true;
  }
  else if (expr.is_name())
  {
    result = expr.get_name();
    return true;
  }

  return false;
}

template<>
bool Querier::get<Expr>(Expr const& expr, Expr& result) const
{
  result = expr;
  return true;
}

bool Querier::find(std::vector<std::string> const& elements, Expr& result) const
{
  auto expr = _root;
  for (auto const& element : elements)
  {
    if (element.empty())
      continue;

    if (!expr.is_table())
      return false;

    Expr key = Parse(element);
    if (key.is_name())
      key = Expr::FromString(key.get_name());

    auto const& table = *expr.get_table();
    auto it = table.find(key);
    if (it == table.end())
      return false;

    expr = it->second;
  }

  result = expr;
  return true;
}

}
