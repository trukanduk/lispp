#pragma once

#include <lispp/virtual_machine_base.h>
#include <lispp/string_tokenizer.h>

namespace lispp {

template<typename TokenizerType = StringTokenizer>
class VirtualMachine : public VirtualMachineBase {
public:
  template<typename... TokenizerArgs>
  VirtualMachine(const std::shared_ptr<Scope>& global_scope,
                 const TokenizerArgs&... tokenizer_args)
      : VirtualMachineBase(global_scope, nullptr) {
    tokenizer_.reset(new TokenizerType(tokenizer_args...));
    init(tokenizer_.get());
  }

  template<typename... TokenizerArgs>
  VirtualMachine(TokenizerArgs&... tokenizer_args)
      : VirtualMachineBase(nullptr) {
    tokenizer_.reset(new TokenizerType(tokenizer_args...));
    init(tokenizer_.get());
  }

  TokenizerType& get_tokenizer() { return *tokenizer_; }

private:
  std::unique_ptr<TokenizerType> tokenizer_;
};

template<>
class VirtualMachine<StringTokenizer> : public VirtualMachineBase {
public:
  VirtualMachine(const std::shared_ptr<Scope>& global_scope,
                 const std::string& initial_code = "")
      : VirtualMachineBase(global_scope, nullptr) {
    tokenizer_.reset(new StringTokenizer(initial_code));
    init(tokenizer_.get());
  }

  VirtualMachine(const std::string& initial_code = "")
      : VirtualMachineBase(nullptr) {
    tokenizer_.reset(new StringTokenizer(initial_code));
    init(tokenizer_.get());
  }

  using VirtualMachineBase::parse;

  ObjectPtr<> parse(const std::string& code) {
    get_tokenizer().clear();
    get_tokenizer() << code;
    return parse();
  }

  using VirtualMachineBase::eval;

  ObjectPtr<> eval(const std::string& code) {
    get_tokenizer().clear();
    get_tokenizer() << code;
    return eval();
  }

  using VirtualMachineBase::eval_all;

  ObjectPtr<> eval_all(const std::string& code) {
    get_tokenizer().clear();
    get_tokenizer() << code;
    return eval_all();
  }

  StringTokenizer& get_tokenizer() { return *tokenizer_; }

private:
  std::unique_ptr<StringTokenizer> tokenizer_;
};

}
