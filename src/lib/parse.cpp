#include "parse.hpp"

#include "util.hpp"

namespace afct {

std::list<std::string> Lex(std::string const& text)
{
  std::list<std::string> result;
  std::string token;

  auto PushToken = [&]() {
    if (token.size())
    {
      result.push_back(token);
      token.clear();
    }
  };

  bool in_comment = false;
  bool in_string = false;
  bool in_table_def = false;
  for (auto c : text)
  {
    if (in_comment)
    {
      if (c == '\n')
        in_comment = false;
      continue;
    }
    else if (c == ';')
    {
      if (in_string)
        token += c;
      else
        in_comment = true;
    }
    else if (c == '"')
    {
      token += c;
      if (in_string)
        PushToken();
      in_string = !in_string;
    }
    else if (in_string)
    {
      token += c;
    }
    else if (c == '#')
    {
      PushToken();
      token += c;
      in_table_def = true;
    }
    else if (in_table_def)
    {
      if (std::isspace(c))
        continue;

      AFCT_CHECK(c == '(', "Expected table definition #(");

      token += c;
      PushToken();
      in_table_def = false;
    }
    else if (c == '\'')
    {
      token = c;
      PushToken();
    }
    else if (c == '(')
    {
      result.push_back("(");
    }
    else if (c == ')')
    {
      PushToken();
      result.push_back(")");
    }
    else if (std::isspace(c))
    {
      PushToken();
    }
    else
    {
      token += c;
    }
  }
  PushToken();

  AFCT_CHECK(!in_table_def, "Incomplete table definition");

  AFCT_CHECK(!in_string, "Incomplete string");

  return result;
}

Expr Parse(std::list<std::string>& tokens)
{
  if (!tokens.size())
    return Expr{};
  auto token = tokens.front();
  tokens.pop_front();

  if (token == "'")
  {
    AFCT_CHECK(!tokens.empty(), "Expected something after '");

    return Expr{List{Expr{Name{"quote"}}, Parse(tokens)}};
  }
  if (token == "#(")
  {
    AFCT_CHECK(!tokens.empty(), "Expected something after #(");

    tokens.push_front("(");
    auto list = Parse(tokens).get_list();
    AFCT_CHECK(
        list.size() % 2 == 0,
        "Expected even arg count when constructing table");

    Table table;
    for (size_t i = 0; i < list.size(); i += 2)
      table[list[i]] = list[i + 1];
    return Expr{std::move(table)};
  }
  else if (token == "(")
  {
    AFCT_CHECK(!tokens.empty(), "Expected )");

    List list;
    while (!tokens.empty() && tokens.front() != ")")
      list.push_back(Parse(tokens));

    AFCT_CHECK(!tokens.empty(), "Expected )");
    tokens.pop_front();

    return Expr{list};
  }
  else
  {
    if (token == "null")
      return Expr{};
    if (token == "true")
      return Expr{true};
    else if (token == "false")
      return Expr{false};
    else
    {
      try
      {
        if (token.find('.') != std::string::npos)
        {
          double d = std::stod(token);
          return Expr{d};
        }
        else
        {
          int64_t i = std::stoll(token);
          return Expr{i};
        }
      }
      catch (std::exception const&)
      {
        if (token.size() >= 2 && token.front() == '"' && token.back() == '"')
          return Expr{String{std::string(
              token.begin() + 1, token.begin() + token.size() - 1)}};
        else
          return Expr{Name{token}};
      }
    }
  }
}

Expr Parse(std::string text)
{
  auto tokens = Lex(std::move(text));
  auto expr = Parse(tokens);
  AFCT_CHECK(tokens.empty(), "Unexpected tokens at end of input");
  return expr;
}

} // namespace afct
