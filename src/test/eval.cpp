#include "lib/eval.hpp"

#include "lib/parse.hpp"
#include "lib/prelude.hpp"
#include "util.hpp"
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

using namespace afct;

BOOST_AUTO_TEST_CASE(eval_nothing)
{
  BOOST_TEST(EvalSimple("") == Expr{});
}

BOOST_AUTO_TEST_CASE(eval_types)
{
  BOOST_TEST(EvalSimple("true") == Expr{true});
  BOOST_TEST(EvalSimple("false") == Expr{false});
  BOOST_TEST(EvalSimple("2.7") == Expr{2.7});
  BOOST_TEST(EvalSimple("2") == Expr{2});
  BOOST_TEST(EvalSimple(R"("string")") == Expr{String{"string"}});
  BOOST_TEST(EvalSimple("'name") == Expr{Name{"name"}});
  BOOST_TEST(EvalSimple("(lambda (x) x)").is_lambda() == true);
  BOOST_TEST(EvalSimple("car").is_builtin() == true);
  BOOST_TEST(EvalSimple("'(1 2)").is_list() == true);
  BOOST_TEST(EvalSimple("#(1 2 3 4)").is_table() == true);
  BOOST_TEST(EvalSimple("#(1 2 3 (+ 2 2))") == EvalSimple("#(1 2 3 4)"));
  BOOST_TEST(EvalSimple("#(1 2 1 3)") == EvalSimple("#(1 3)"));
}

BOOST_AUTO_TEST_CASE(eval_builtin)
{
  BOOST_TEST(EvalSimple("(define x 1)") == Expr{1});

  auto code = R"(
    (begin
      (define f
        (lambda (x)
          (if (= x 2)
            (+ x 10)
            x)))
      (apply f '(2))))";
  BOOST_TEST(EvalSimple(code) == Expr{12});
}

BOOST_AUTO_TEST_CASE(eval_begin_scoping)
{
  auto env = Prelude();
  std::string code = "(begin (define a 2) (define b 3) (* a b))";
  BOOST_TEST(Eval(code, env) == Expr{6});
  BOOST_TEST(env->find("a").value() == Expr{2});
  BOOST_TEST(env->find("b").value() == Expr{3});
}

BOOST_AUTO_TEST_CASE(eval_first_class_function_arg)
{
  auto env = Prelude();

  auto f = "(define f (lambda (to-call a) (to-call (* a 2))))";
  Eval(std::string(f), env);

  auto g = "(define g (lambda (n) (* n 4)))";
  Eval(std::string(g), env);

  auto run = std::string("(f g 2)");
  BOOST_TEST(Eval(run, env) == Expr{16});
}

BOOST_AUTO_TEST_CASE(eval_lambda_env)
{
  auto env = Prelude();

  auto b = "(define b 1)";
  Eval(std::string(b), env);

  auto f = "(define f (lambda (n) (* n a b)))";
  Eval(std::string(f), env);

  auto a = "(define a 3)";
  Eval(std::string(a), env);

  auto run = std::string("(f 2)");

  BOOST_TEST(Eval(run, env) == Expr{6});

  auto a2 = "(define a 2)";
  Eval(std::string(a2), env);

  BOOST_TEST(Eval(run, env) == Expr{4});

  auto n = "(define n 10)";
  Eval(std::string(n), env);

  BOOST_TEST(Eval(run, env) == Expr{4});

  auto b2 = "(define b 2)";
  Eval(std::string(b2), env);

  BOOST_TEST(Eval(run, env) == Expr{8});
}

BOOST_AUTO_TEST_CASE(eval_closure_simple)
{
  auto env = Prelude();

  auto f = "(define f (lambda () (begin (define a 1) (lambda () a))))";
  Eval(std::string(f), env);

  auto g = "(define g (f))";
  Eval(std::string(g), env);

  auto run = std::string("(g)");

  BOOST_TEST(Eval(run, env) == Expr{1});

  auto outer_def = "(define a 2)";
  Eval(std::string(outer_def), env);

  BOOST_TEST(Eval(run, env) == Expr{1});
}

BOOST_AUTO_TEST_CASE(eval_closure_function)
{
  auto env = Prelude();

  auto f = "(define f (lambda (to-call) (lambda (a) (to-call (* a 2)))))";
  Eval(std::string(f), env);

  auto g = "(define g (lambda (n) (* n 4)))";
  Eval(std::string(g), env);

  auto h = "(define h (f g))";
  Eval(std::string(h), env);

  auto run = std::string("(h 2)");

  BOOST_TEST(Eval(run, env) == Expr{16});

  auto outer_def = "(define to-call (lambda (n) (* n 8)))";
  Eval(std::string(outer_def), env);

  BOOST_TEST(Eval(run, env) == Expr{16});
}

BOOST_AUTO_TEST_CASE(eval_recursion_tco)
{
  auto env = Prelude();

  std::string sum = R"(
    (define sum (lambda (x a) (if (= x 0) a (sum (- x 1) (+ a 1))))))";
  Eval(sum, env);

  std::string run = "(sum 10000 0)";
  BOOST_TEST(Eval(run, env) == Expr{10000});
  BOOST_TEST(env->find("x").has_value() == false);
}

BOOST_AUTO_TEST_CASE(eval_recursive_definition_update)
{
  auto env = Prelude();

  auto gen_one =
      "(define gen (lambda (v i) (if (= i 0) v (gen (append v 1) (- i 1)))))";
  Eval(std::string(gen_one), env);

  BOOST_TEST(Eval(std::string("(gen '() 3)"), env) == Parse("(1 1 1)"));

  auto gen_two =
      "(define gen (lambda (v i) (if (= i 0) v (gen (append v 2) (- i 1)))))";
  Eval(std::string(gen_two), env);

  BOOST_TEST(Eval(std::string("(gen '() 3)"), env) == Parse("(2 2 2)"));
}

BOOST_AUTO_TEST_CASE(eval_double_recursion)
{
  auto env = Prelude();

  auto fib = R"(
    (define fib
      (lambda (n)
        (if (< n 2)
        n
        (+ (fib (- n 1)) (fib (- n 2)))))))";
  Eval(std::string(fib), env);
  BOOST_TEST(Eval(std::string("(fib 15)"), env) == Expr{610});
}

BOOST_AUTO_TEST_CASE(eval_nested_envs)
{
  auto env = Prelude();

  auto f = R"(
    (define f
      (lambda (a b)
        (begin
          (define g (lambda (c d) (= c d)))
          (g a b)))))";
  Eval(std::string(f), env);
  BOOST_TEST(Eval(std::string("(f 1 2)"), env) == Expr{false});
}

BOOST_AUTO_TEST_CASE(eval_stdlib_math)
{
  BOOST_TEST(EvalSimple("(= 1 1)") == Expr{true});
  BOOST_TEST(EvalSimple("(+ 1 1)") == Expr{2});
  BOOST_TEST(EvalSimple("(- 1 1)") == Expr{0});
  BOOST_TEST(EvalSimple("(* 2 3)") == Expr{6});
  BOOST_TEST(EvalSimple("(/ 12 2)") == Expr{6});
  BOOST_TEST(EvalSimple("(< 1 2)") == Expr{true});
  BOOST_TEST(EvalSimple("(< 2 1)") == Expr{false});
  BOOST_TEST(EvalSimple("(> 1 2)") == Expr{false});
  BOOST_TEST(EvalSimple("(> 2 1)") == Expr{true});
  BOOST_TEST(EvalSimple("(and false true)") == Expr{false});
  BOOST_TEST(EvalSimple("(and true true)") == Expr{true});
  BOOST_TEST(EvalSimple("(or false true)") == Expr{true});
  BOOST_TEST(EvalSimple("(or false false)") == Expr{false});
  BOOST_TEST(EvalSimple("(not false)") == Expr{true});
  BOOST_TEST(EvalSimple("(not true)") == Expr{false});
  BOOST_TEST(EvalSimple("(min '(3 6 5 1 4 8 7))") == Expr{1});
  BOOST_TEST(EvalSimple("(max '(3 6 5 1 4 8 7))") == Expr{8});
}

BOOST_AUTO_TEST_CASE(eval_stdlib_structure)
{
  BOOST_TEST(EvalSimple("(list 1 (+ 1 1))") == EvalSimple("'(1 2)"));
  BOOST_TEST(EvalSimple("(table 1 2 3 (+ 2 2))") == EvalSimple("#(1 2 3 4)"));
  BOOST_TEST(EvalSimple("(table 1 2 1 3)") == EvalSimple("#(1 3)"));
  BOOST_TEST(EvalSimple("(length '(1 2))") == Expr{2});
  BOOST_TEST(EvalSimple("(length #(1 2 3 4 5 6))") == Expr{3});
  BOOST_TEST(EvalSimple("(append '(1 2 3 4) 5)") == EvalSimple("'(1 2 3 4 5)"));
  BOOST_TEST(EvalSimple("(cons '(1 2) '(3 4))") == Parse("((1 2) (3 4))"));
  BOOST_TEST(EvalSimple("(car '(1 2 3))") == Expr{1});
  BOOST_TEST(EvalSimple("(cdr '(1 2 3))") == Parse("(2 3)"));
  BOOST_TEST(EvalSimple("(get #(1 2 3 4) 1)") == Expr{2});
  BOOST_TEST(
      EvalSimple("(begin (define a #(1 2 3 4)) (set! a 3 5) (get a 3))") ==
      Expr{5});
  BOOST_TEST(
      EvalSimple("(begin (define a #(1 2 3 4)) (set! a 3 5))") == Expr{5});
  auto keys = EvalSimple("(keys #(1 2 3 4))").get_list();
  bool keys_ok =
      keys == List{Expr{1}, Expr{3}} || keys == List{Expr{3}, Expr{1}};
  BOOST_TEST(keys_ok);
  auto values = EvalSimple("(values #(1 2 3 4))").get_list();
  bool values_ok =
      values == List{Expr{2}, Expr{4}} || values == List{Expr{4}, Expr{2}};
  BOOST_TEST(values_ok);
}

BOOST_AUTO_TEST_CASE(eval_stdlib_types)
{
  BOOST_TEST(
      EvalSimple(R"((cat  "hello " "world"))") == Parse(R"("hello world")"));
  BOOST_TEST(EvalSimple("(bool 2)") == Expr{true});
  BOOST_TEST(EvalSimple("(double 2)") == Expr{2.0});
  BOOST_TEST(EvalSimple("(int 2.7)") == Expr{2});
  BOOST_TEST(EvalSimple("(string 2.7)") == Expr{String{"2.7"}});
}

BOOST_AUTO_TEST_CASE(eval_stdlib_functional)
{
  BOOST_TEST(EvalSimple("(apply (lambda (x y) (* x y)) '(2 3))") == Expr{6});
  BOOST_TEST(
      EvalSimple("(map (lambda (x) (* x 2)) '(1 2 3))") == Parse("(2 4 6)"));
  BOOST_TEST(
      EvalSimple("(filter (lambda (x) (= x 2)) '(1 2 3))") == Parse("(2)"));
}
