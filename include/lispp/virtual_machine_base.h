#pragma once

#include <lispp/parser.h>
#include <lispp/tokenizer.h>
#include <lispp/scope.h>

namespace lispp {

class VirtualMachineBase {
public:
  ObjectPtr<> parse();
  ObjectPtr<> eval();
  ObjectPtr<> eval_all();

  Parser& get_parser();
  std::shared_ptr<Scope> get_global_scope();

protected:
  explicit VirtualMachineBase(ITokenizer* tokenizer);
  VirtualMachineBase(const std::shared_ptr<Scope>& global_scope,
                     ITokenizer* tokenizer);

  void init(ITokenizer* tokenizer);

  ITokenizer* get_tokenizer_base();

private:
  ITokenizer* tokenizer_;
  std::unique_ptr<Parser> parser_;
  std::shared_ptr<Scope> global_scope_;
};

} // lispp
