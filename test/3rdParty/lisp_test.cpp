#include "lisp_test.h"

#include <lispp/function_utils.h>

void LispTest::SetUp() {
    vm_.reset(new lispp::VirtualMachine<>());
}

void LispTest::TearDown() {
    vm_.reset();
}

// evaluate expression and compare result to expected
void LispTest::ExpectEq(const std::string& expression, std::string result) {
    lispp::ObjectPtr<> eval_expression = vm_->eval(expression);
    // lispp::ObjectPtr<> eval_result = vm_->parse(result);
    std::string eval_string = (eval_expression.valid() ? eval_expression->to_string() : "()");

    EXPECT_EQ(result, eval_string);
}

// evaluate expression and check that there is not errors
void LispTest::ExpectNoError(const std::string& expression) {
    // EXPECT_NO_THROW(vm_->eval(expression));
    vm_->eval(expression);
}

// check that expression evaluation produces syntax error
void LispTest::ExpectSyntaxError(const std::string& expression) {
    bool thrown = false;
    try {
        vm_->eval(expression);
    } catch (const lispp::TokenizerError& e) {
        thrown = true;
    } catch (const lispp::ParserError& e) {
        thrown = true;
    } catch (const lispp::MacroArgumentsError& e) {
        thrown = true;
    } catch (...) {}

    EXPECT_TRUE(thrown) << "Wasn't thrown any exception!" << std::endl
        << "(expected TokenizerError or ParserError)";
}

// check that expression evaluation produces runtime error
void LispTest::ExpectRuntimeError(const std::string& expression) {
    EXPECT_THROW(vm_->eval(expression), lispp::ExecutionError);
}

// check that expression evaluation produces name error
void LispTest::ExpectUndefinedVariable(const std::string& expression) {
    EXPECT_THROW(vm_->eval(expression), lispp::ScopeError);
}
