include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)
add_executable(test
  builder.cpp
  eval.cpp
  expr.cpp
  function.cpp
  lex.cpp
  main.cpp
  parse.cpp
  querier.cpp
  util.cpp
  visitor.cpp)
target_link_libraries(test artifact stdc++fs fmt)
