cmake_minimum_required(VERSION 3.24)

project(msgpack_perf
    VERSION 0.0.1
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)

FetchContent_Declare(
  glaze
  GIT_REPOSITORY https://github.com/stephenberry/glaze.git
  GIT_TAG main
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(glaze)

file(GLOB_RECURSE srcs include/*.hpp)

add_executable(${PROJECT_NAME} ${srcs} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE glaze::glaze)
target_include_directories(${PROJECT_NAME} PRIVATE include ../msgpack-c/include)
