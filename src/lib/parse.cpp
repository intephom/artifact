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

  bool in_string = false;
  bool in_comment = false;
  for (auto c : text)
  {
    if (in_comment)
    {
      if (c == '\n')
        in_comment = false;
      else
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
    if (tokens.empty() || tokens.front() != "(")
      AFCT_ERROR("Expected ( after '");
    tokens.pop_front();

    auto list = std::make_shared<List>();
    list->push_back(Expr::FromName("quote"));
    auto sublist = std::make_shared<List>();
    while (!tokens.empty() && tokens.front() != ")")
    {
      sublist->push_back(Parse(tokens));
    }
    list->push_back(Expr::FromList(sublist));

    if (tokens.empty())
      AFCT_ERROR("Expected )");
    tokens.pop_front();

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
          return Expr::FromString(std::string(token.begin() + 1, token.begin() + token.size() - 1));
        else
          return Expr::FromName(token);
      }
    }
  }
}

Expr Parse(std::string const& text)
{
  auto tokens = Lex(text);
  return Parse(tokens);
}

}
