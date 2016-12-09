#pragma once

#include <lispp/object.h>

namespace lispp {

class BooleanObject : public Object {
public:
  BooleanObject() = default;
  explicit BooleanObject(bool value) : value_(value) {}
  ~BooleanObject() {}

  static std::string GetTypeName() {
    return "boolean";
  }

  bool get_value() const { return value_; }
  void set_value(bool value) { value_ = value; }

  BooleanObject* as_boolean() override { return this; }
  const BooleanObject* as_boolean() const override { return this; }

  bool operator==(const Object& other) const override {
    const auto* other_identifier = other.as_boolean();
    return (other_identifier != nullptr && other_identifier->value_ == value_);
  }

  std::string to_string() const override {
    return (value_ ? "#t" : "#f");
  }

  ObjectPtr<> eval(const std::shared_ptr<Scope>&) override {
    return this;
  }

protected:
  bool value_ = false;
};

inline std::ostream& operator<<(std::ostream& out, const BooleanObject& obj) {
  return (out << obj.to_string());
}

} // lispp
