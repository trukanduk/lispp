#include <lispp/virtual_machine_base.h>

#include <lispp/builtins.h>

namespace lispp {

ObjectPtr<> VirtualMachineBase::parse() {
  try {
    return parser_->parse_object();
  } catch (const ParserError&) {
    tokenizer_->clear();
    throw;
  }
}

ObjectPtr<> VirtualMachineBase::eval() {
  auto object = parse();
  return object.safe_eval(global_scope_);
}

ObjectPtr<> VirtualMachineBase::eval_all() {
  ObjectPtr<> result;
  while (parser_->has_objects()) {
    result = eval();
  }

  return result;
}

Parser& VirtualMachineBase::get_parser() {
  return *parser_;
}
std::shared_ptr<Scope> VirtualMachineBase::get_global_scope() {
  return global_scope_;
}

VirtualMachineBase::VirtualMachineBase(
    const std::shared_ptr<Scope>& global_scope, ITokenizer* tokenizer) {
  init(tokenizer);

  global_scope_ = global_scope;
}

VirtualMachineBase::VirtualMachineBase(ITokenizer* tokenizer) {
  init(tokenizer);

  global_scope_.reset(new Scope);
  init_global_scope(global_scope_);
}

void VirtualMachineBase::init(ITokenizer* tokenizer) {
  tokenizer_ = tokenizer;
  parser_.reset(new Parser(tokenizer_));
}

ITokenizer* VirtualMachineBase::get_tokenizer_base() {
  return tokenizer_;
}

} // lispp
