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

      if (c != '(')
        AFCT_ERROR("Expected table definition #(");

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

  if (in_table_def)
    AFCT_ERROR("Incomplete table definition");

  if (in_string)
    AFCT_ERROR("Incomplete string");

  return result;
}

Expr Parse(std::list<std::string>& tokens)
{
  if (!tokens.size())
    return Expr::FromNull();
  auto token = tokens.front();
  tokens.pop_front();

  if (token == "'")
  {
    if (tokens.empty())
      AFCT_ERROR("Expected something after '");

    auto list = std::make_shared<List>();
    list->push_back(Expr::FromName("quote"));
    list->push_back(Parse(tokens));
    return Expr::FromList(list);
  }
  if (token == "#(")
  {
    if (tokens.empty())
      AFCT_ERROR("Expected something after #(");

    auto list = std::make_shared<List>();
    list->push_back(Expr::FromName("table"));

    tokens.push_front("(");
    auto pairs = Parse(tokens);
    for (auto pair : *pairs.get_list())
      list->push_back(pair);

    return Expr::FromList(list);
  }
  else if (token == "(")
  {
    if (tokens.empty())
      AFCT_ERROR("Expected )");

    auto list = std::make_shared<List>();
    while (!tokens.empty() && tokens.front() != ")")
    {
      list->push_back(Parse(tokens));
    }

    if (tokens.empty())
      AFCT_ERROR("Expected )");
    tokens.pop_front();

    return Expr::FromList(list);
  }
  else
  {
    if (token == "null")
      return Expr::FromNull();
    if (token == "true")
      return Expr::FromBool(true);
    else if (token == "false")
      return Expr::FromBool(false);
    else
    {
      try
      {
        if (token.find('.') != std::string::npos)
        {
          double d = std::stod(token);
          return Expr::FromDouble(d);
        }
        else
        {
          uint64_t i = std::stoll(token);
          return Expr::FromInt(i);
        }
      }
      catch (std::exception const&)
      {
        if (token.size() >= 2 && token.front() == '"' && token.back() == '"')
          return Expr::FromString(
              std::string(token.begin() + 1, token.begin() + token.size() - 1));
        else
          return Expr::FromName(token);
      }
    }
  }
}

Expr Parse(std::string text)
{
  auto tokens = Lex(std::move(text));
  auto expr = Parse(tokens);
  if (!tokens.empty())
    AFCT_ERROR("Unexpected tokens at end of input");
  return expr;
}

} // namespace afct
