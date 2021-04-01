#include "expr.hpp"

#include "eval.hpp"
#include "function.hpp"
#include "parse.hpp"
#include "util.hpp"
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

using namespace afct;

class DummyFunction : public IFunction
{
public:
  std::string name() const final
  {
    return "dummy";
  }

  Expr call(List const& args) final
  {
    return Expr{};
  }
};

BOOST_AUTO_TEST_CASE(expr_is)
{
  BOOST_TEST(Expr{}.is_null() == true);
  BOOST_TEST(Expr{true}.is_bool() == true);
  BOOST_TEST(Expr{2.7}.is_double() == true);
  BOOST_TEST(Expr{2}.is_int() == true);
  BOOST_TEST(Expr{String{"string"}}.is_string() == true);
  BOOST_TEST(Expr{Name{"+"}}.is_name() == true);
  BOOST_TEST(Expr{std::make_shared<DummyFunction>()}.is_function() == true);
  BOOST_TEST(Parse("(1 2)").is_list() == true);
  BOOST_TEST(EvalSimple("#(1 2)").is_table() == true);
}

BOOST_AUTO_TEST_CASE(expr_get)
{
  BOOST_TEST(Expr{true}.get_bool() == true);
  BOOST_TEST(Expr{false}.get_bool() == false);
  BOOST_TEST(Expr{2.7}.get_double() == 2.7);
  BOOST_TEST(Expr{2}.get_int() == 2);
  BOOST_TEST(Expr{String{"string"}}.get_string() == "string");
  BOOST_TEST(Expr{Name{"+"}}.get_name() == "+");
  auto f = std::make_shared<DummyFunction>();
  BOOST_TEST(Expr{f}.get_function() == f);
  auto l = List{Expr{1}, Expr{2}};
  BOOST_TEST(*Parse("(1 2)").get_list() == l);
  auto t = Table({{Expr{1}, Expr{2}}});
  BOOST_TEST(*EvalSimple("#(1 2)").get_table() == t);
}

BOOST_AUTO_TEST_CASE(expr_truthy)
{
  BOOST_TEST(Expr{}.truthy() == false);
  BOOST_TEST(Expr{true}.truthy() == true);
  BOOST_TEST(Expr{false}.truthy() == false);
  BOOST_TEST(Expr{2.7}.truthy() == true);
  BOOST_TEST(Expr{0.0}.truthy() == false);
  BOOST_TEST(Expr{2}.truthy() == true);
  BOOST_TEST(Expr{0}.truthy() == false);
  BOOST_TEST(Expr{String{"string"}}.truthy() == true);
  BOOST_TEST(Expr{String{""}}.truthy() == false);
  BOOST_TEST(Expr{Name{"+"}}.truthy() == true);
  BOOST_TEST(Expr{Name{""}}.truthy() == false);
  BOOST_TEST(Expr{std::make_shared<DummyFunction>()}.truthy() == true);
  BOOST_TEST(Expr{std::shared_ptr<IFunction>()}.truthy() == false);
  BOOST_TEST(EvalSimple("'(1 2)").truthy() == true);
  BOOST_TEST(EvalSimple("'()").truthy() == false);
  BOOST_TEST(EvalSimple("#(1 2)").truthy() == true);
  BOOST_TEST(EvalSimple("#()").truthy() == false);
}

BOOST_AUTO_TEST_CASE(expr_is_quote)
{
  BOOST_TEST(IsQuote(Parse("(quote (1 1))")) == true);
  BOOST_TEST(IsQuote(Parse("'(1 1)")) == true);
  BOOST_TEST(IsQuote(Parse("(print 27)")) == false);
  BOOST_TEST(IsQuote(Parse("#(1 2 3 4)")) == false);
}

BOOST_AUTO_TEST_CASE(expr_unquote)
{
  BOOST_TEST(Unquote(Parse("(quote (1 1))")) == Parse("(1 1)"));
  BOOST_TEST(Unquote(Parse("'(1 1)")) == Parse("(1 1)"));
  BOOST_TEST(Unquote(Parse("(print 27)")) == Parse("(print 27)"));
  BOOST_TEST(
      Unquote(Parse("#(1 '(1 2) 3 '(3 4))")) == Parse("#(1 '(1 2) 3 '(3 4))"));
}

BOOST_AUTO_TEST_CASE(expr_ostream)
{
  auto code =
      R"((null () true false 2. 2.7 .7 2 "hello" + (1 2 '(3 4)) (quote (1 2))))";
  auto sugared =
      R"((null () true false 2 2.7 0.7 2 "hello" + (1 2 '(3 4)) '(1 2)))";
  auto expr = Parse(code);
  std::ostringstream stream;
  stream << expr;

  BOOST_TEST(stream.str() == sugared);
}

BOOST_AUTO_TEST_CASE(expr_ostream_table)
{
  auto code = R"((table 1 2 3 4))";
  auto expr = EvalSimple(code);
  auto sugared_ordered = R"(#(1 2 3 4))";
  auto sugared_unordered = R"(#(3 4 1 2))";
  std::ostringstream stream;
  stream << expr;

  bool ok =
      stream.str() == sugared_ordered || stream.str() == sugared_unordered;
  BOOST_TEST(ok);
}
