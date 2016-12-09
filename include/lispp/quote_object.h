#pragma once

#include <lispp/object.h>
#include <lispp/object_ptr.h>

namespace lispp {

class QuoteObject : public DecoratorObject {
public:
  QuoteObject() = default;
  explicit QuoteObject(ObjectPtr<> value) : value_(value) {}
  ~QuoteObject() {}

  static std::string GetTypeName() {
    return "quote";
  }

  ObjectPtr<> get_value() { return value_; }
  const ObjectPtr<> get_value() const { return value_; }
  void set_value(const ObjectPtr<>& value) { value_ = value; }

  QuoteObject* as_quote() override { return this; }
  const QuoteObject* as_quote() const override { return this; }

  bool operator==(const Object& other) const override {
    const auto* other_quote = other.as_quote();
    return (other_quote != nullptr && value_.safe_equal(other_quote->value_));
  }

  std::string to_string() const override {
    return "'" + (value_.valid() ? value_->to_string() : "nil");
  }

  ObjectPtr<> eval(const std::shared_ptr<Scope>&) override {
    return value_;
  }

private:
  ObjectPtr<> value_;
};

inline std::ostream& operator<<(std::ostream& out, const QuoteObject& obj) {
  return (out << obj.to_string());
}

} // lispp
