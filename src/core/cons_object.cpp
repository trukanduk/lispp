#include <lispp/cons_object.h>

#include <lispp/scope.h>
#include <lispp/callable_object.h>

namespace lispp {

bool ConsObject::operator==(const Object& other) const {
  const auto* other_cons = other.as_cons();
  return (other_cons != nullptr &&
          left_value_.safe_equal(other_cons->left_value_) &&
          right_value_.safe_equal(other_cons->right_value_));
}

std::string ConsObject::to_string() const {
  if (!left_value_.valid() && !right_value_.valid()) {
    return "()";
  } else {
    std::stringstream ss;
    ss << "(";
    print_as_tail(ss);
    return ss.str();
  }
}

ObjectPtr<> ConsObject::eval(const std::shared_ptr<Scope>& scope) {
  if (!left_value_.valid()) {
    throw ExecutionError("Cannot execute empty list");
  }

  ObjectPtr<> evaled_value(left_value_->eval(scope));
  if (!evaled_value.valid()) {
    throw ExecutionError(left_value_->to_string() + " is not callable");
  }
  ObjectPtr<CallableObject> callable(evaled_value->as_callable());

  if (!callable.valid()) {
    throw ExecutionError(evaled_value->to_string() + " is not callable");
  }

  return callable->execute(scope, right_value_);
}

void ConsObject::print_as_tail(std::ostream& out) const {
  out << (left_value_.valid() ? left_value_->to_string() : "nil");

  if (!right_value_.valid()) {
    out << ")";
  } else if (right_value_->as_cons()) {
    out << " ";
    right_value_->as_cons()->print_as_tail(out);
  } else {
    out << " . " << (right_value_.valid() ? right_value_->to_string() : "nil")
        << ")";
  }
}

} // lispp
