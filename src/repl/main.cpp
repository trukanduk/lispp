#include <iostream>
#include <memory>

#include <lispp/object.h>
#include <lispp/object_ptr.h>
#include <lispp/istream_tokenizer.h>
#include <lispp/file_tokenizer.h>
#include <lispp/parser.h>
#include <lispp/scope.h>
#include <lispp/virtual_machine.h>
#include <lispp/function_utils.h>

#define CONTEST_MODE

// TODO: command objects
#ifndef CONTEST_MODE
void RunAsRepl() {
  lispp::VirtualMachine<lispp::IstreamTokenizer> vm(std::cin);

  std::cout << "> ";
  while (vm.get_parser().has_objects()) {
    try {
      auto result_object = vm.eval();
      if (result_object.valid()) {
        std::cout << *result_object << std::endl;
      }
    } catch (const lispp::TokenizerError& e) {
      std::cout << "TokenizerError: " << e.what() << std::endl;
    } catch (const lispp::ParserError& e) {
      std::cout << "ParserError: " << e.what() << std::endl;
    } catch (const lispp::MacroArgumentsError& e) {
      std::cout << "MacroArgumentsError: " << e.what() << std::endl;
    } catch (const lispp::ExecutionError& e) {
      std::cout << "ExecutionError: " << e.what() << std::endl;
    } catch (const lispp::ScopeError& e) {
      std::cout << "ScopeError: " << e.what() << std::endl;
    } catch (const std::exception& e) {
      std::cout << "Unknown exception: " << e.what() << std::endl;
    } catch (...) {
      std::cout << "Unknown error." << std::endl;
    }

    std::cout << "> ";
  }
}
#else // CONTEST_MODE
void RunAsRepl() {
  lispp::VirtualMachine<lispp::IstreamTokenizer> vm(std::cin);

  while (vm.get_parser().has_objects()) {
    try {
      auto result_object = vm.eval();
      if (result_object.valid()) {
        std::cout << *result_object << std::endl;
      } else {
        std::cout << "()" << std::endl;
      }
    } catch (const lispp::TokenizerError& e) {
      std::cout << "syntax error" << std::endl;
      return;

    } catch (const lispp::ParserError& e) {
      std::cout << "syntax error" << std::endl;
      return;

    } catch (const lispp::MacroArgumentsError& e) {
      std::cout << "syntax error" << std::endl;
      return;

    } catch (const lispp::ExecutionError& e) {
      std::cout << "runtime error" << std::endl;

    } catch (const lispp::ScopeError& e) {
      std::cout << "name error" << std::endl;

    } catch (const std::exception& e) {
      std::cout << "runtime error" << std::endl;

    } catch (...) {
      std::cout << "runtime error" << std::endl;
    }
  }
}
#endif

void RunFromFile(const std::string filename) {
  lispp::VirtualMachine<lispp::FileTokenizer> vm(filename);

  try {
    vm.eval_all();
  } catch (const lispp::TokenizerError& e) {
    std::cout << "TokenizerError at line "
              << vm.get_tokenizer().get_current_line() << ": "
              << e.what() << std::endl;
  } catch (const lispp::ParserError& e) {
    std::cout << "ParserError at line "
              << vm.get_tokenizer().get_current_line() << ": "
              << e.what() << std::endl;
  } catch (const lispp::ExecutionError& e) {
    std::cout << "ExecutionError: " << e.what() << std::endl;
  } catch (const lispp::ScopeError& e) {
    std::cout << "ScopeError: " << e.what() << std::endl;
  } catch (const std::exception& e) {
    std::cout << "Unknown exception: " << e.what() << std::endl;
  } catch (...) {
    std::cout << "Unknown error." << std::endl;
  }
}

int main(int argc, const char* argv[]) {
  if (argc == 1) {
    RunAsRepl();
  } else {
    RunFromFile(argv[1]);
  }

  return 0;
}
