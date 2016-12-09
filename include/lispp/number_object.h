#pragma once

#include <lispp/object.h>

namespace lispp {

class NumberObject : public Object {
public:
  NumberObject() = default;
  explicit NumberObject(double value) : value_(value) {}
  ~NumberObject() {}

  static std::string GetTypeName() {
    return "number";
  }

  double get_value() const { return value_; }
  void set_value(double value) { value_ = value; }

  NumberObject* as_number() override { return this; }
  const NumberObject* as_number() const override { return this; }

  bool operator==(const Object& other) const override {
    const auto* other_number = other.as_number();
    return (other_number != nullptr && other_number->value_ == value_);
  }

  std::string to_string() const override {
    return std::to_string(value_);
  }

  ObjectPtr<> eval(const std::shared_ptr<Scope>&) override { return this; }

protected:
  double value_ = 0;
};

// TODO: define a macro?
inline std::ostream& operator<<(std::ostream& out, const NumberObject& obj) {
  return (out << obj.to_string());
}

} // lispp
