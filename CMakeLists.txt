cmake_minimum_required(VERSION 3.10)

project(cloth_sim VERSION 1.0)

file(GLOB_RECURSE SOURCES  "src/*.cpp")

add_executable(cloth_sim ${SOURCES})

#target_link_libraries(cloth_sim sfml-graphics sfml-window sfml-system)

set_property(TARGET cloth_sim PROPERTY CXX_STANDARD 20)

target_link_libraries(cloth_sim glfw)

if(UNIX)
  target_link_libraries(cloth_sim GLEW)
  target_link_libraries(cloth_sim GL)
endif(UNIX)
