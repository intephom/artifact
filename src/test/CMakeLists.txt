add_executable(test
  builder.cpp
  eval.cpp
  expr.cpp
  lex.cpp
  main.cpp
  parse.cpp
  querier.cpp
  util.cpp
  visitor.cpp)
target_link_libraries(test artifact stdc++fs)
