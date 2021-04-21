#pragma once

#include "env.hpp"

namespace afct {

Expr Eq(List const& args);
Expr Add(List const& args);
Expr Sub(List const& args);
Expr Mult(List const& args);
Expr Div(List const& args);
Expr LessThan(List const& args);
Expr GreaterThan(List const& args);
Expr And(List const& args);
Expr Or(List const& args);
Expr Not(List const& args);
Expr Min(List const& args);
Expr Max(List const& args);
Expr ToList(List const& args);
Expr ToTable(List const& args);
Expr Length(List const& args);
Expr Append(List const& args);
Expr Cons(List const& args);
Expr Car(List const& args);
Expr Cdr(List const& args);
Expr Cat(List const& args);
Expr Get(List const& args);
Expr SetBang(List const& args);
Expr Keys(List const& args);
Expr Values(List const& args);
Expr Bool(List const& args);
Expr Double(List const& args);
Expr Int(List const& args);
Expr ToString(List const& args);
Expr Apply(List const& args);
Expr Map(List const& args);
Expr Filter(List const& args);
Expr Print(List const& args);
Expr GetEnv(List const& args);
Expr Rand(List const& args);

Env Prelude();

} // namespace afct
