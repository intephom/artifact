#include "visitor.hpp"

#include "eval.hpp"
#include "function.hpp"
#include "prelude.hpp"
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <sstream>

using namespace afct;

class Visitor : public IVisitor
{
public:
  std::ostringstream stream;

  void null_value() override
  {
    stream << "null";
  }

  void bool_value(bool value) override
  {
    stream << "bool " << value;
  }

  void double_value(double value) override
  {
    stream << "double " << value;
  }

  void int_value(int64_t value) override
  {
    stream << "int " << value;
  }

  void string_value(std::string const& value) override
  {
    stream << "string " << value;
  }

  void name_value(std::string const& value) override
  {
    stream << "name " << value;
  }

  void function_value(std::shared_ptr<IFunction> const& value) override
  {
    stream << "function " << value->name();
  }

  void start_list(size_t size) override
  {
    stream << "start list size " << size;
  }

  void end_list() override
  {
    stream << "end list";
  }

  void start_table(size_t size) override
  {
    stream << "start table size " << size;

  }

  void start_key() override
  {
    stream << "start key";
  }

  void end_key() override
  {
    stream << "end key";
  }

  void end_table() override
  {
    stream << "end table";
  }
};

BOOST_AUTO_TEST_CASE(visitor)
{
  auto code =
      R"((list null true false 2.7 27 "hello" 'lambda (lambda (a) (* a 2)) '(1 2) '#(1 2)))";
  auto env = afct::Prelude();
  auto expr = Eval(std::string(code), env);

  Visitor visitor;
  Visit(expr, &visitor);
  auto string = visitor.stream.str();

  auto expected = "start list size 10"
                  "null"
                  "bool 1"
                  "bool 0"
                  "double 2.7"
                  "int 27"
                  "string hello"
                  "name lambda"
                  "function lambda"
                  "start list size 2"
                  "int 1"
                  "int 2"
                  "end list"
                  "start table size 1"
                  "start key"
                  "int 1"
                  "end key"
                  "int 2"
                  "end table"
                  "end list";
  BOOST_TEST(string == expected);
}
