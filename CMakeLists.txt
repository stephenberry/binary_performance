cmake_minimum_required(VERSION 3.24)

project(binary_perf
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

FetchContent_Declare(
  jsoncons
  GIT_REPOSITORY https://github.com/danielaparker/jsoncons.git
  GIT_TAG master
  GIT_SHALLOW TRUE
)

FetchContent_Declare(
  zpp_bits
  GIT_REPOSITORY https://github.com/eyalz800/zpp_bits.git
  GIT_TAG main
  GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(glaze jsoncons zpp_bits)

file(GLOB_RECURSE srcs include/*.hpp)

add_executable(${PROJECT_NAME} ${srcs} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE glaze::glaze)
target_include_directories(${PROJECT_NAME} PRIVATE
    include
    ../msgpack-c/include
    ${jsoncons_SOURCE_DIR}/include
    ${zpp_bits_SOURCE_DIR}
)
