#include <lispp/back_tick_object.h>

#include <functional>

#include <lispp/scope.h>
#include <lispp/list_utils.h>

namespace lispp {

bool BackTickObject::operator==(const Object& other) const {
  const auto* other_back_tick = other.as_back_tick();
  return (other_back_tick != nullptr &&
          value_.safe_equal(other_back_tick->value_));
}

std::string BackTickObject::to_string() const {
  return "`" + (value_.valid() ? value_->to_string() : "nil");
}

ObjectPtr<> BackTickObject::eval(const std::shared_ptr<Scope>& scope) {
  return map_list(
      value_,
      std::bind(process_object_on_eval, std::placeholders::_1, scope));
}

ObjectPtr<> BackTickObject::process_object_on_eval(
    ObjectPtr<> object, const std::shared_ptr<Scope>& scope) {
  if (object == nullptr) {
    return nullptr;
  } else if (object->as_comma()) {
    return object->eval(scope);
  } else {
    return object;
  }
}

std::ostream& operator<<(std::ostream& out, const BackTickObject& obj) {
  return (out << obj.to_string());
}

} // lispp
