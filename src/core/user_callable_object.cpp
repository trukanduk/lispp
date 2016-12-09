#include <lispp/user_callable_object.h>

#include <sstream>

#include <lispp/list_utils.h>

namespace lispp {

UserCallableObject::UserCallableObject(
    const std::string& name, const std::vector<std::string>& args,
    const std::vector<ObjectPtr<>>& body, const std::shared_ptr<Scope>& closure,
    const std::string& rest_arg_name, CallableType type)
    : CallableObject(type, true), name_(name), args_(args), body_(body),
    closure_(closure), rest_arg_name_(rest_arg_name) {}

ObjectPtr<> UserCallableObject::execute_impl(
    const std::shared_ptr<Scope>& scope,
    const std::vector<ObjectPtr<>>& args) {
  if (args.size() < args_.size() ||
      (rest_arg_name_.empty() && args.size() > args_.size())) {
    std::stringstream ss;
    ss << name_ << " "
       << (get_type() == CallableType::kFunction ? "function" : "macro")
       << " expects " << args_.size() << " arguments but " << args.size() << " given";
    throw ExecutionError(ss.str());
  }

  std::shared_ptr<Scope> local_scope = closure_->create_child_scope();
  for (std::size_t arg_index = 0; arg_index < args_.size(); ++arg_index) {
    local_scope->set_value(args_[arg_index], args[arg_index]);
  }

  if (!rest_arg_name_.empty()) {
    std::vector<ObjectPtr<>> rest_args(
        std::next(args.begin(), args_.size()), args.end());
    auto rest_lst = pack_list(rest_args);
    local_scope->set_value(rest_arg_name_, rest_lst);
  }

  ObjectPtr<> result;
  for (auto body_expression : body_) {
    result = body_expression.safe_eval(local_scope);
    // NOTE: this scope? It's strange place but macroses are evaluated twice?
    if (get_type() == CallableType::kMacro) {
      result = result.safe_eval(scope);
    }
  }

  return result;
}

} // lispp
