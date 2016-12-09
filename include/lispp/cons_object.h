#pragma once

#include <sstream>

#include <lispp/object.h>
#include <lispp/object_ptr.h>

namespace lispp {

class ConsObject : public Object {
public:
  ConsObject() = default;
  // NOTE: useful for incremental list construction
  explicit ConsObject(ObjectPtr<> left_value) : left_value_(left_value) {}
  ConsObject(ObjectPtr<> left_value, ObjectPtr<> right_value)
      : left_value_(left_value), right_value_(right_value) {}
  ~ConsObject() {}

  static std::string GetTypeName() {
    return "cons";
  }

  ObjectPtr<> get_left_value() { return left_value_; }
  const ObjectPtr<> get_left_value() const { return left_value_; }
  void set_left_value(const ObjectPtr<>& left_value) {
    left_value_ = left_value;
  }

  ObjectPtr<> get_right_value() { return right_value_; }
  const ObjectPtr<> get_right_value() const { return right_value_; }
  void set_right_value(const ObjectPtr<>& right_value) {
    right_value_ = right_value;
  }

  ConsObject* as_cons() override { return this; }
  const ConsObject* as_cons() const override { return this; }

  bool operator==(const Object& other) const override;
  std::string to_string() const override;
  ObjectPtr<> eval(const std::shared_ptr<Scope>& scope) override;

private:
  void print_as_tail(std::ostream& out) const;

  ObjectPtr<> left_value_;
  ObjectPtr<> right_value_;
};

inline std::ostream& operator<<(std::ostream& out, const ConsObject& obj) {
  return (out << obj.to_string());
}

} // lispp
