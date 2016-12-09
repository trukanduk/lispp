#pragma once

#include <string>

#include <lispp/object.h>
#include <lispp/scope.h>

namespace lispp {

// FIXME: make common superclass for this and StringObject ?
class SymbolObject : public Object {
public:
  SymbolObject() = default;
  explicit SymbolObject(const std::string& value) : value_(value) {}
  ~SymbolObject() {}

  static std::string GetTypeName() {
    return "symbol";
  }

  const std::string& get_value() const { return value_; }
  void set_value(const std::string& value) { value_ = value; }

  SymbolObject* as_symbol() override { return this; }
  const SymbolObject* as_symbol() const override { return this; }

  bool operator==(const Object& other) const override {
    const auto* other_identifier = other.as_symbol();
    return (other_identifier != nullptr && other_identifier->value_ == value_);
  }

  std::string to_string() const override {
    return value_;
  }

  ObjectPtr<> eval(const std::shared_ptr<Scope>& scope) override {
    return scope->get_value(value_);
  }

protected:
  std::string value_;
};

inline std::ostream& operator<<(std::ostream& out, const SymbolObject& obj) {
  return (out << obj.to_string());
}

} // lispp
