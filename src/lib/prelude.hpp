#pragma once

#include "env.hpp"

namespace afct {

Expr Eq(List& args, std::shared_ptr<Env>);
Expr Add(List& args, std::shared_ptr<Env>);
Expr Sub(List& args, std::shared_ptr<Env>);
Expr Mult(List& args, std::shared_ptr<Env>);
Expr Div(List& args, std::shared_ptr<Env>);
Expr LessThan(List& args, std::shared_ptr<Env>);
Expr GreaterThan(List& args, std::shared_ptr<Env>);
Expr And(List& args, std::shared_ptr<Env>);
Expr Or(List& args, std::shared_ptr<Env>);
Expr Not(List& args, std::shared_ptr<Env>);
Expr Min(List& args, std::shared_ptr<Env>);
Expr Max(List& args, std::shared_ptr<Env>);
Expr ToList(List& args, std::shared_ptr<Env>);
Expr ToTable(List& args, std::shared_ptr<Env>);
Expr Length(List& args, std::shared_ptr<Env>);
Expr Append(List& args, std::shared_ptr<Env>);
Expr Cons(List& args, std::shared_ptr<Env>);
Expr Car(List& args, std::shared_ptr<Env>);
Expr Cdr(List& args, std::shared_ptr<Env>);
Expr Cat(List& args, std::shared_ptr<Env>);
Expr Get(List& args, std::shared_ptr<Env>);
Expr SetBang(List& args, std::shared_ptr<Env>);
Expr Keys(List& args, std::shared_ptr<Env>);
Expr Values(List& args, std::shared_ptr<Env>);
Expr Bool(List& args, std::shared_ptr<Env>);
Expr Double(List& args, std::shared_ptr<Env>);
Expr Int(List& args, std::shared_ptr<Env>);
Expr ToString(List& args, std::shared_ptr<Env>);
Expr Apply(List& args, std::shared_ptr<Env> env);
Expr Map(List& args, std::shared_ptr<Env> env);
Expr Filter(List& args, std::shared_ptr<Env> env);
Expr Print(List& args, std::shared_ptr<Env>);
Expr GetEnv(List& args, std::shared_ptr<Env>);
Expr Rand(List& args, std::shared_ptr<Env>);

std::shared_ptr<Env> Prelude();

} // namespace afct
