#include "parse.hpp"
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

using namespace afct;

BOOST_AUTO_TEST_CASE(parse_nothing)
{
  auto code = "";
  auto expr = Expr::FromNull();

  BOOST_TEST(Parse(code) == expr);
}

BOOST_AUTO_TEST_CASE(parse_quote_sugar)
{
  auto code = "'1";
  auto list = {Expr::FromName("quote"), Expr::FromInt(1)};
  auto expr = Expr::FromList(std::make_shared<List>(list));

  BOOST_TEST(Parse(code) == expr);
}

BOOST_AUTO_TEST_CASE(parse_quote)
{
  auto code = "(quote 1)";
  auto list = {Expr::FromName("quote"), Expr::FromInt(1)};
  auto expr = Expr::FromList(std::make_shared<List>(list));

  BOOST_TEST(Parse(code) == expr);
}

BOOST_AUTO_TEST_CASE(parse_bad_quote)
{
  auto code = "'";

  BOOST_CHECK_THROW(Parse(code), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(parse_table_sugar)
{
  auto code = "#('(1 2) '(3 4))";
  auto first = Parse("'(1 2)");
  auto second = Parse("'(3 4)");
  auto list = {Expr::FromName("table"), first, second};
  auto expr = Expr::FromList(std::make_shared<List>(list));

  BOOST_TEST(Parse(code) == expr);
}

BOOST_AUTO_TEST_CASE(parse_table)
{
  auto code = "(table '(1 2) '(3 4))";
  auto first = Parse("'(1 2)");
  auto second = Parse("'(3 4)");
  auto list = {Expr::FromName("table"), first, second};
  auto expr = Expr::FromList(std::make_shared<List>(list));

  BOOST_TEST(Parse(code) == expr);
}

BOOST_AUTO_TEST_CASE(parse_bad_table)
{
  auto code = "#";

  BOOST_CHECK_THROW(Parse(code), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(parse_empty_table)
{
  auto code = "#()";
  auto list = {Expr::FromName("table")};
  auto expr = Expr::FromList(std::make_shared<List>(list));

  BOOST_TEST(Parse(code) == expr);
}

BOOST_AUTO_TEST_CASE(parse_incomplete_table)
{
  auto code = "#('(1 2)";

  BOOST_CHECK_THROW(Parse(code), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(parse_list)
{
  auto code = "(+ 1 2)";
  auto list = {Expr::FromName("+"), Expr::FromInt(1), Expr::FromInt(2)};
  auto expr = Expr::FromList(std::make_shared<List>(list));

  BOOST_TEST(Parse(code) == expr);
}

BOOST_AUTO_TEST_CASE(parse_sublist)
{
  auto code = "(+ 1 (+ 2 3))";
  auto sublist = {Expr::FromName("+"), Expr::FromInt(2), Expr::FromInt(3)};
  auto list = {
      Expr::FromName("+"),
      Expr::FromInt(1),
      Expr::FromList(std::make_shared<List>(sublist))};
  auto expr = Expr::FromList(std::make_shared<List>(list));

  BOOST_TEST(Parse(code) == expr);
}

BOOST_AUTO_TEST_CASE(parse_unended_list)
{
  auto code = "(+ 1 2";

  BOOST_CHECK_THROW(Parse(code), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(parse_unstarted_list)
{
  auto code = "+ 1 2)";

  BOOST_CHECK_THROW(Parse(code), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(parse_types)
{
  auto code =
      R"((null true false 2. 2.7 .7 2 "hello" + (1 2) '(1 2) #('(1 2))))";
  auto sublist = {Expr::FromInt(1), Expr::FromInt(2)};
  auto subquote = {
      Expr::FromName("quote"), Expr::FromList(std::make_shared<List>(sublist))};
  auto subtable = {
      Expr::FromName("table"),
      Expr::FromList(std::make_shared<List>(subquote))};
  auto list = {
      Expr::FromNull(),
      Expr::FromBool(true),
      Expr::FromBool(false),
      Expr::FromDouble(2.0),
      Expr::FromDouble(2.7),
      Expr::FromDouble(0.7),
      Expr::FromInt(2),
      Expr::FromString("hello"),
      Expr::FromName("+"),
      Expr::FromList(std::make_shared<List>(sublist)),
      Expr::FromList(std::make_shared<List>(subquote)),
      Expr::FromList(std::make_shared<List>(subtable))};
  auto expr = Expr::FromList(std::make_shared<List>(list));

  BOOST_TEST(Parse(code) == expr);
}
