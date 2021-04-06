set(sources
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
add_library(objects OBJECT ${sources})
set_property(TARGET objects PROPERTY POSITION_INDEPENDENT_CODE 1)
add_library(artifact_shared SHARED $<TARGET_OBJECTS:objects>)
set_target_properties(artifact_shared PROPERTIES OUTPUT_NAME artifact)
add_library(artifact_static STATIC $<TARGET_OBJECTS:objects>)
set_target_properties(artifact_static PROPERTIES OUTPUT_NAME artifact)
