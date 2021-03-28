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
  auto expr = Expr::FromList({Expr::FromName("quote"), Expr::FromInt(1)});

  BOOST_TEST(Parse(code) == expr);
}

BOOST_AUTO_TEST_CASE(parse_quote)
{
  auto code = "(quote 1)";
  auto expr = Expr::FromList({Expr::FromName("quote"), Expr::FromInt(1)});

  BOOST_TEST(Parse(code) == expr);
}

BOOST_AUTO_TEST_CASE(parse_bad_quote)
{
  auto code = "'";

  BOOST_CHECK_THROW(Parse(code), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(parse_table)
{
  auto code = "#(1 '(3 4))";
  auto first = Parse("1");
  auto second = Parse("'(3 4)");
  auto expr = Expr::FromTable(Table{{first, second}});

  BOOST_TEST(Parse(code) == expr);
}

BOOST_AUTO_TEST_CASE(parse_table_noeval)
{
  auto code = "#(1 (+ 3 4))";
  auto first = Parse("1");
  auto second = Parse("(+ 3 4)");
  auto expr = Expr::FromTable(Table{{first, second}});

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
  auto expr = Expr::FromTable(Table{{}});

  BOOST_TEST(Parse(code) == expr);
}

BOOST_AUTO_TEST_CASE(parse_incomplete_table)
{
  auto code = "#(1 2";

  BOOST_CHECK_THROW(Parse(code), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(parse_list_key_table)
{
  auto code = "#((+ 1 1) 2)";

  BOOST_CHECK_THROW(Parse(code), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(parse_list)
{
  auto code = "(+ 1 2)";
  auto expr = Expr::FromList({Expr::FromName("+"), Expr::FromInt(1), Expr::FromInt(2)});

  BOOST_TEST(Parse(code) == expr);
}

BOOST_AUTO_TEST_CASE(parse_sublist)
{
  auto code = "(+ 1 (+ 2 3))";
  auto expr = Expr::FromList({
      Expr::FromName("+"),
      Expr::FromInt(1),
      Expr::FromList({Expr::FromName("+"), Expr::FromInt(2), Expr::FromInt(3)})});

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

BOOST_AUTO_TEST_CASE(parse_comment)
{
  auto code = R"("hel;lo" ; comm"ent" ; comment)";

  BOOST_TEST(Parse(code) == Expr::FromString("hel;lo"));
}

BOOST_AUTO_TEST_CASE(parse_types)
{
  auto code =
      R"((null true false 2. 2.7 .7 2 "hello" + (1 2) '(1 2) #(1 2)))";
  auto sublist = {Expr::FromInt(1), Expr::FromInt(2)};
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
      Expr::FromList(sublist),
      Expr::FromList({Expr::FromName("quote"), Expr::FromList(sublist)}),
      Expr::FromTable(Table{{afct::Expr::FromInt(1), afct::Expr::FromInt(2)}})};
  auto expr = Expr::FromList(std::move(list));

  BOOST_TEST(Parse(code) == expr);
}
