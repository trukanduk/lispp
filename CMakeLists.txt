project(lispp)
cmake_minimum_required(VERSION 2.8)

# BUILD_TESTS: OFF, BASE, 3PARTY, ALL, ON
if (NOT DEFINED "BUILD_TESTS")
    set(BUILD_TESTS "OFF")
elseif (${BUILD_TESTS} STREQUAL "ON" OR ${BUILD_TESTS} STREQUAL "")
    set(BUILD_TESTS "ALL")
elseif (NOT (${BUILD_TESTS} STREQUAL "OFF" OR
             ${BUILD_TESTS} STREQUAL "BASE" OR
             ${BUILD_TESTS} STREQUAL "3RDPARTY" OR
             ${BUILD_TESTS} STREQUAL "ALL"))
    message (SEND_ERROR "Unknown tests configuration: ${BUILD_TESTS}. Available options are OFF, BASE, 3RDPARTY, ALL, ON (alias for ALL)")
endif ()
option(BUILD_REPL "Build interactive interpreter" ON)

# Hardcore mode on
if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "CLANG")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -pedantic -g -Wno-unused-variable")

  if (${CYGWIN})
      # gnu++11 is neccessary for gtest and dependencies
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")
  else ()
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fsanitize=undefined -fsanitize=address")
  endif ()
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
  # do nothing
  # NOTE: too much warnings...
else ()
  message(WARNING "Unknown compiler!")
endif()

set(3RDPARTY_ROOT "${CMAKE_SOURCE_DIR}/3rdParty")
include_directories(include)

include(StdLibGenerator.cmake)
# include_directories("${CMAKE_BINARY_DIR}/stdlib")

set(GENERATED_STDLIB_SOURCES)
generate_stdlib_source("common" GENERATED_STDLIB_SOURCES)

# TODO: split to subdirectories
set(CORE_SOURCE_DIR src/core)
add_library(lispp_core # FIXME: naming
  ${CORE_SOURCE_DIR}/back_tick_object.cpp
  ${CORE_SOURCE_DIR}/builtins.cpp
  ${CORE_SOURCE_DIR}/callable_object.cpp
  ${CORE_SOURCE_DIR}/cons_object.cpp
  ${CORE_SOURCE_DIR}/function_utils.cpp
  ${CORE_SOURCE_DIR}/istream_tokenizer.cpp
  ${CORE_SOURCE_DIR}/list_utils.cpp
  ${CORE_SOURCE_DIR}/object.cpp
  ${CORE_SOURCE_DIR}/parser.cpp
  ${CORE_SOURCE_DIR}/scope.cpp
  ${CORE_SOURCE_DIR}/string_tokenizer.cpp
  ${CORE_SOURCE_DIR}/token.cpp
  ${CORE_SOURCE_DIR}/tokenizer.cpp
  ${CORE_SOURCE_DIR}/user_callable_object.cpp
  ${CORE_SOURCE_DIR}/virtual_machine_base.cpp

  ${GENERATED_STDLIB_SOURCES}
)

if (${BUILD_REPL})
    set(REPL_SOURCE_DIR src/repl)
    add_executable(lispp
        ${REPL_SOURCE_DIR}/main.cpp
        # ${REPL_SOURCE_DIR}/command_object.cpp
    )
    target_link_libraries(lispp lispp_core)
endif ()

if (NOT ${BUILD_TESTS} STREQUAL "OFF")
    set(GTEST_INCLUDE_DIR ${3RDPARTY_ROOT})
    set(GTEST_SOURCE_DIR "${3RDPARTY_ROOT}/gtest")
    add_library(gtest
        "${GTEST_SOURCE_DIR}/gtest-all.cc"
        "${GTEST_SOURCE_DIR}/gtest_main.cc")
    include_directories(${GTEST_INCLUDE_DIR})

    set(TESTS_SOURCES "")
    if ((${BUILD_TESTS} STREQUAL "BASE") OR (${BUILD_TESTS} STREQUAL "ALL"))
        set (TESTS_SOURCES
            ${TESTS_SOURCES}
            test/base/test_tokenizer.cpp
            test/base/test_string_tokenizer.cpp
            test/base/test_parser.cpp
            test/base/test_scope.cpp
            test/base/test_list_utils.cpp
        )
    endif ()

    if ((${BUILD_TESTS} STREQUAL "3RDPARTY") OR (${BUILD_TESTS} STREQUAL "ALL"))
        set (TESTS_SOURCES
            ${TESTS_SOURCES}
            test/3rdParty/lisp_test.cpp
            test/3rdParty/test_eval.cpp
            test/3rdParty/test_integer.cpp
            test/3rdParty/test_boolean.cpp
            test/3rdParty/test_symbol.cpp
            test/3rdParty/test_list.cpp
            test/3rdParty/test_lambda.cpp
            test/3rdParty/test_control_flow.cpp
        )
    endif ()

    add_executable(test ${TESTS_SOURCES})
    target_link_libraries(test lispp_core gtest)
endif ()
