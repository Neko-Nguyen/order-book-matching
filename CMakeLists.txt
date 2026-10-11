cmake_minimum_required(VERSION 3.20)
project(order_book_matching LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

option(OBM_BUILD_TESTS "Build unit tests" ON)
option(OBM_SANITIZE "Build with AddressSanitizer and UndefinedBehaviorSanitizer" OFF)

add_library(obm_warnings INTERFACE)
if(MSVC)
  target_compile_options(obm_warnings INTERFACE /W4 /permissive-)
else()
  target_compile_options(obm_warnings INTERFACE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()

if(OBM_SANITIZE)
  if(MSVC)
    add_compile_options(/fsanitize=address)
  else()
    add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address,undefined)
  endif()
endif()

add_library(obm src/order_book.cpp)
target_include_directories(obm PUBLIC include)
target_link_libraries(obm PRIVATE obm_warnings)

if(OBM_BUILD_TESTS)
  find_package(GTest QUIET)
  if(NOT GTest_FOUND)
    include(FetchContent)
    FetchContent_Declare(
      googletest
      URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.tar.gz
      DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    # Match the project's runtime library on MSVC.
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
  endif()

  enable_testing()
  add_executable(obm_tests tests/order_book_test.cpp)
  target_link_libraries(obm_tests PRIVATE obm obm_warnings GTest::gtest_main)
  include(GoogleTest)
  gtest_discover_tests(obm_tests)
endif()
