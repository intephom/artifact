include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_library(artifact SHARED
  artifact.cpp
  builder.cpp
  env.cpp
  eval.cpp
  expr.cpp
  function.cpp
  parse.cpp
  prelude.cpp
  querier.cpp
  util.cpp
  visitor.cpp)
