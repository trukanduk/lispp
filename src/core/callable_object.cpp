#include <lispp/callable_object.h>

#include <lispp/cons_object.h>
#include <lispp/list_utils.h>

namespace lispp {

std::string CallableObject::to_string() const {
  if (type_ == CallableType::kFunction) {
    return "<procedure>";
  } else {
    return "<macro>";
  }
}

ObjectPtr<> CallableObject::execute(const std::shared_ptr<Scope>& scope,
                                    const ObjectPtr<>& args) {
  ObjectPtr<> prepared_args(prepare_args(args, scope));

  std::shared_ptr<Scope> local_scope = scope;
  if (create_separate_scope_) {
    local_scope = scope->create_child_scope();
  }

  auto unpacked_args = unpack_list(prepared_args);

  return execute_impl(local_scope, unpacked_args);
}

ObjectPtr<> CallableObject::prepare_args(
    const ObjectPtr<>& src_args, const std::shared_ptr<Scope>& scope) const {

  if (type_ == CallableType::kFunction) {
    ObjectPtr<> sources(src_args);
    return map_list(src_args, [&scope](ObjectPtr<> object) {
        return object->eval(scope);
    });
  } else {
    return src_args;
  }
}

std::ostream& operator<<(std::ostream& out, const CallableObject& obj) {
  return (out << obj.to_string());
}

} // lispp
