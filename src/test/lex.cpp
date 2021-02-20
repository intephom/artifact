#include "parse.hpp"
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

using namespace afct;

BOOST_AUTO_TEST_CASE(lex_nothing)
{
  BOOST_TEST(Lex("") == std::list<std::string>());
}

BOOST_AUTO_TEST_CASE(lex_expr)
{
  auto code = R"((+ 1 1))";
  auto lexed = std::list<std::string>{"(", "+", "1", "1", ")"};

  BOOST_TEST(Lex(code) == lexed);
}

BOOST_AUTO_TEST_CASE(lex_whitespace)
{
  auto code = "( \n + 1  \n   1)";
  auto lexed = std::list<std::string>{"(", "+", "1", "1", ")"};

  BOOST_TEST(Lex(code) == lexed);
}

BOOST_AUTO_TEST_CASE(lex_comment_own_line)
{
  auto code = R"(
    (begin
      (+ 1 1)
      ; comment
      (+ 2 2)))";
  auto lexed = std::list<std::string>{
      "(", "begin", "(", "+", "1", "1", ")", "(", "+", "2", "2", ")", ")"};

  BOOST_TEST(Lex(code) == lexed);
}

BOOST_AUTO_TEST_CASE(lex_comment_in_line)
{
  auto code = R"(
    (begin
      (+ 1 1) ; comment
      (+ 2 2)))";
  auto lexed = std::list<std::string>{
      "(", "begin", "(", "+", "1", "1", ")", "(", "+", "2", "2", ")", ")"};

  BOOST_TEST(Lex(code) == lexed);
}

BOOST_AUTO_TEST_CASE(lex_comment_stress)
{
  auto code = R"(
    (begin
      (+ 1 1) ; comment ;comment
      ; comment "comment
      (+ 2 2)))";
  auto lexed = std::list<std::string>{
      "(", "begin", "(", "+", "1", "1", ")", "(", "+", "2", "2", ")", ")"};

  BOOST_TEST(Lex(code) == lexed);
}

BOOST_AUTO_TEST_CASE(lex_string)
{
  auto code = R"((print "hello" "world"))";
  auto lexed =
      std::list<std::string>{"(", "print", R"("hello")", R"("world")", ")"};

  BOOST_TEST(Lex(code) == lexed);
}

BOOST_AUTO_TEST_CASE(lex_incomplete_string)
{
  auto code = R"((print "hello" "wor))";

  BOOST_CHECK_THROW(Lex(code), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(lex_table)
{
  auto code = R"((print #('(1 2) '(3 4))))";
  auto lexed = std::list<std::string>{
      "(",
      "print",
      "#(",
      "'",
      "(",
      "1",
      "2",
      ")",
      "'",
      "(",
      "3",
      "4",
      ")",
      ")",
      ")"};

  BOOST_TEST(Lex(code) == lexed);
}

BOOST_AUTO_TEST_CASE(lex_incomplete_table)
{
  auto code = R"((print #'(1 2) '(3 4))))";

  BOOST_CHECK_THROW(Lex(code), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(lex_whitespace_table)
{
  auto code = R"((print#
      ('(1 2)) #  ('(3 4))))";
  auto lexed = std::list<std::string>{
      "(",
      "print",
      "#(",
      "'",
      "(",
      "1",
      "2",
      ")",
      ")",
      "#(",
      "'",
      "(",
      "3",
      "4",
      ")",
      ")",
      ")"};

  BOOST_TEST(Lex(code) == lexed);
}

BOOST_AUTO_TEST_CASE(lex_quote)
{
  auto code = R"((begin '(1 2)))";
  auto lexed =
      std::list<std::string>{"(", "begin", "'", "(", "1", "2", ")", ")"};

  BOOST_TEST(Lex(code) == lexed);
}
