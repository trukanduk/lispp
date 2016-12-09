#pragma once

#include <string>

#include <gtest/gtest.h>

#include <lispp/object.h>
#include <lispp/object_ptr.h>
#include <lispp/virtual_machine.h>

struct LispTest : public ::testing::Test {
    // initialize new interpreter here
    LispTest() = default;

    void SetUp();
    void TearDown();

    // evaluate expression and compare result to expected
    void ExpectEq(const std::string& expression, std::string result);

    // evaluate expression and check that there is not errors
    void ExpectNoError(const std::string& expression);

    // check that expression evaluation produces syntax error
    void ExpectSyntaxError(const std::string& expression);

    // check that expression evaluation produces runtime error
    void ExpectRuntimeError(const std::string& expression);

    // check that expression evaluation produces name error
    void ExpectUndefinedVariable(const std::string& expression);

protected:
    std::unique_ptr<lispp::VirtualMachine<>> vm_;
};