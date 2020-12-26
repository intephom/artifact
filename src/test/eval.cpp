#include "eval.hpp"
#include "parse.hpp"
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

using namespace afct;

BOOST_AUTO_TEST_CASE(eval_nothing)
{
  BOOST_TEST(Eval(std::string("")) == Expr::FromNull());
}

BOOST_AUTO_TEST_CASE(eval_types)
{
  BOOST_TEST(Eval(std::string("true")) == Expr::FromBool(true));
  BOOST_TEST(Eval(std::string("false")) == Expr::FromBool(false));
  BOOST_TEST(Eval(std::string("2.7")) == Expr::FromDouble(2.7));
  BOOST_TEST(Eval(std::string("2")) == Expr::FromInt(2));
  BOOST_TEST(Eval(std::string(R"("string")")) == Expr::FromString("string"));
  BOOST_TEST(Eval(std::string("'name")) == Expr::FromName("name"));
  BOOST_TEST(Eval(std::string("car")).is_function() == true);
  BOOST_TEST(Eval(std::string("'(1 2)")).is_list() == true);
  BOOST_TEST(Eval(std::string("#(1 2 3 4)")).is_table() == true);
}

BOOST_AUTO_TEST_CASE(eval_builtin)
{
  auto code = R"(
    (begin
      (define f
        (lambda (x)
          (if (= x 2)
            (+ x 10)
            x)))
      (apply f '(2))))";
  BOOST_TEST(Eval(std::string(code)) == Expr::FromInt(12));
}

BOOST_AUTO_TEST_CASE(eval_stdlib_math)
{
  BOOST_TEST(Eval(std::string("(= 1 1)")) == Expr::FromBool(true));
  BOOST_TEST(Eval(std::string("(+ 1 1)")) == Expr::FromInt(2));
  BOOST_TEST(Eval(std::string("(- 1 1)")) == Expr::FromInt(0));
  BOOST_TEST(Eval(std::string("(* 2 3)")) == Expr::FromInt(6));
  BOOST_TEST(Eval(std::string("(/ 12 2)")) == Expr::FromInt(6));
  BOOST_TEST(Eval(std::string("(< 1 2)")) == Expr::FromBool(true));
  BOOST_TEST(Eval(std::string("(< 2 1)")) == Expr::FromBool(false));
  BOOST_TEST(Eval(std::string("(> 1 2)")) == Expr::FromBool(false));
  BOOST_TEST(Eval(std::string("(> 2 1)")) == Expr::FromBool(true));
  BOOST_TEST(Eval(std::string("(and false true)")) == Expr::FromBool(false));
  BOOST_TEST(Eval(std::string("(and true true)")) == Expr::FromBool(true));
  BOOST_TEST(Eval(std::string("(or false true)")) == Expr::FromBool(true));
  BOOST_TEST(Eval(std::string("(or false false)")) == Expr::FromBool(false));
  BOOST_TEST(Eval(std::string("(not false)")) == Expr::FromBool(true));
  BOOST_TEST(Eval(std::string("(not true)")) == Expr::FromBool(false));
}

BOOST_AUTO_TEST_CASE(eval_stdlib_structure)
{
  BOOST_TEST(Eval(std::string("(list 1 2)")) == Eval(std::string("'(1 2)")));
  BOOST_TEST(Eval(std::string("(table 1 2 3 4)")) == Eval(std::string("#(1 2 3 4)")));
  BOOST_TEST(Eval(std::string("(length '(1 2))")) == Expr::FromInt(2));
  BOOST_TEST(Eval(std::string("(append '(1 2 3 4) 5)")) == Eval(std::string("'(1 2 3 4 5)")));
  BOOST_TEST(Eval(std::string("(cons '(1 2) '(3 4))")) == Parse("((1 2) (3 4))"));
  BOOST_TEST(Eval(std::string("(car '(1 2 3))")) == Expr::FromInt(1));
  BOOST_TEST(Eval(std::string("(cdr '(1 2 3))")) == Parse("(2 3)"));
}

BOOST_AUTO_TEST_CASE(eval_stdlib_types)
{
  BOOST_TEST(Eval(std::string(R"((cat  "hello " "world"))")) == Parse(R"("hello world")"));
  BOOST_TEST(Eval(std::string("(get #(1 2 3 4) 1)")) == Expr::FromInt(2));
  BOOST_TEST(Eval(std::string("(begin (define a #(1 2 3 4)) (set! a 3 5) (get a 3))")) == Expr::FromInt(5));
  BOOST_TEST(Eval(std::string("(bool 2)")) == Expr::FromBool(true));
  BOOST_TEST(Eval(std::string("(double 2)")) == Expr::FromDouble(2));
  BOOST_TEST(Eval(std::string("(int 2.7)")) == Expr::FromInt(2));
  BOOST_TEST(Eval(std::string("(string 2.7)")) == Expr::FromString("2.7"));
}

BOOST_AUTO_TEST_CASE(eval_stdlib_functional)
{
  BOOST_TEST(Eval(std::string("(apply (lambda (x y) (* x y)) '(2 3))")) == Expr::FromInt(6));
  BOOST_TEST(Eval(std::string("(map (lambda (x) (* x 2)) '(1 2 3))")) == Parse("(2 4 6)"));
  BOOST_TEST(Eval(std::string("(filter (lambda (x) (= x 2)) '(1 2 3))")) == Parse("(2)"));
}
