#pragma once

#include <lispp/object.h>
#include <lispp/object_ptr.h>

#include <lispp/scope.h>

namespace lispp {

class CommaObject : public DecoratorObject {
public:
  CommaObject() = default;
  explicit CommaObject(ObjectPtr<> value) : value_(value) {}
  ~CommaObject() {}

  static std::string GetTypeName() {
    return "comma";
  }

  ObjectPtr<> get_value() { return value_; }
  const ObjectPtr<> get_value() const { return value_; }
  void set_value(const ObjectPtr<>& value) { value_ = value; }

  CommaObject* as_comma() override { return this; }
  const CommaObject* as_comma() const override { return this; }

  bool operator==(const Object& other) const override {
    const auto* other_comma = other.as_comma();
    return (other_comma != nullptr && value_.safe_equal(other_comma->value_));
  }

  std::string to_string() const override {
    return "," + (value_.valid() ? value_->to_string() : "nil");
  }

  ObjectPtr<> eval(const std::shared_ptr<Scope>& scope) override {
    return (value_.valid() ? value_->eval(scope) : nullptr);
  }

protected:
  ObjectPtr<> value_;
};

inline std::ostream& operator<<(std::ostream& out, const CommaObject& obj) {
  return (out << obj.to_string());
}

} // lispp
