#include "parse.hpp"
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

using namespace afct;

BOOST_AUTO_TEST_CASE(parse_nothing)
{
  auto code = "";
  auto expr = Expr{};

  BOOST_TEST(Parse(code) == expr);
}

BOOST_AUTO_TEST_CASE(parse_quote_sugar)
{
  auto code = "'1";
  auto expr = Expr{List{Expr{Name{"quote"}}, Expr{1}}};

  BOOST_TEST(Parse(code) == expr);
}

BOOST_AUTO_TEST_CASE(parse_quote)
{
  auto code = "(quote 1)";
  auto expr = Expr{List{Expr{Name{"quote"}}, Expr{1}}};

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
  auto expr = Expr{Table{{first, second}}};

  BOOST_TEST(Parse(code) == expr);
}

BOOST_AUTO_TEST_CASE(parse_table_noeval)
{
  auto code = "#(1 (+ 3 4))";
  auto first = Parse("1");
  auto second = Parse("(+ 3 4)");
  auto expr = Expr{Table{{first, second}}};

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
  auto expr = Expr{Table{}};

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
  auto expr = Expr{{Expr{Name{"+"}}, Expr{1}, Expr{2}}};

  BOOST_TEST(Parse(code) == expr);
}

BOOST_AUTO_TEST_CASE(parse_sublist)
{
  auto code = "(+ 1 (+ 2 3))";
  auto expr = Expr{
      {Expr{Name{"+"}}, Expr{1}, Expr{{Expr{Name{"+"}}, Expr{2}, Expr{3}}}}};

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

  BOOST_TEST(Parse(code) == Expr{String{"hel;lo"}});
}

BOOST_AUTO_TEST_CASE(parse_types)
{
  auto code = R"((null true false 2. 2.7 .7 2 "hello" + (1 2) '(1 2) #(1 2)))";
  auto sublist = {Expr{1}, Expr{2}};
  auto list = {
      Expr{},
      Expr{true},
      Expr{false},
      Expr{2.0},
      Expr{2.7},
      Expr{0.7},
      Expr{2},
      Expr{String{"hello"}},
      Expr{Name{"+"}},
      Expr{sublist},
      Expr{List{Expr{Name{"quote"}}, Expr{sublist}}},
      Expr{Table{{Expr{1}, Expr{2}}}}};
  auto expr = Expr{std::move(list)};

  BOOST_TEST(Parse(code) == expr);
}
