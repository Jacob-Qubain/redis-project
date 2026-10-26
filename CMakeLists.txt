cmake_minimum_required(VERSION 3.20)
project(mini_redis LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Turn on with: cmake -B build -DSANITIZE=ON
option(SANITIZE "Build with AddressSanitizer + UndefinedBehaviorSanitizer" OFF)
if(SANITIZE)
  add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
  add_link_options(-fsanitize=address,undefined)
endif()

add_compile_options(-Wall -Wextra -Wpedantic)

# Core library: everything except main(), so tests can link it.
add_library(core src/hash_table.cpp)
target_include_directories(core PUBLIC src)

add_executable(mini-redis src/main.cpp)
target_link_libraries(mini-redis PRIVATE core)

add_executable(bench bench/bench.cpp)
target_link_libraries(bench PRIVATE core)

# Tests (Catch2 is downloaded automatically the first time you configure)
include(FetchContent)
FetchContent_Declare(Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.7.1)
FetchContent_MakeAvailable(Catch2)

add_executable(tests tests/test_hash_table.cpp)
target_link_libraries(tests PRIVATE core Catch2::Catch2WithMain)
enable_testing()
add_test(NAME tests COMMAND tests)
