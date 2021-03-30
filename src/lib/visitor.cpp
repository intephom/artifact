#include "util.hpp"
#include "visitor.hpp"

namespace afct {

void Visit(afct::Expr const& expr, IVisitor* visitor)
{
  auto type = expr.get_type();

  if (type == Type::Null)
    visitor->null_value();
  else if (type == Type::Bool)
    visitor->bool_value(expr.get_bool());
  else if (type == Type::Double)
    visitor->double_value(expr.get_double());
  else if (type == Type::Int)
    visitor->int_value(expr.get_int());
  else if (type == Type::String)
    visitor->string_value(expr.get_string());
  else if (type == Type::Name)
    visitor->name_value(expr.get_name());
  else if (type == Type::Function)
    visitor->function_value(expr.get_function());
  else if (type == Type::List)
  {
    auto const& list = *expr.get_list();
    visitor->start_list(list.size());
    for (auto const& element : list)
      Visit(element, visitor);
    visitor->end_list();
  }
  else if (type == Type::Table)
  {
    auto const& table = *expr.get_table();
    visitor->start_table(table.size());
    for (auto const& pair : table)
    {
      visitor->start_key();
      Visit(pair.first, visitor);
      visitor->end_key();
      Visit(pair.second, visitor);
    }
    visitor->end_table();
  }
  else
  {
    AFCT_ERROR("Visiting unknown type");
  }
}

} // namespace afct
