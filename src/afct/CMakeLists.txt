include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../lib)
add_executable(afct main.cpp)
target_link_libraries(afct artifact stdc++fs fmt)
