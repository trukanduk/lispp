#pragma once

#include <string>

#include <lispp/object.h>

namespace lispp {

// FIXME: make common superclass for this and IdentifierObject ?
class CharactersObject : public Object {
public:
  CharactersObject() = default;
  explicit CharactersObject(const std::string& value) : value_(value) {}
  ~CharactersObject() {}

  static std::string GetTypeName() {
    return "characters";
  }

  const std::string& get_value() const { return value_; }
  void set_value(const std::string& value) { value_ = value; }

  CharactersObject* as_characters() override { return this; }
  const CharactersObject* as_characters() const override { return this; }

  bool operator==(const Object& other) const override {
    const auto* other_string = other.as_characters();
    return (other_string != nullptr && other_string->value_ == value_);
  }

  std::string to_string() const override {
    return "\"" + value_ + "\"";
  }

  ObjectPtr<> eval(const std::shared_ptr<Scope>&) override { return this; }

protected:
  std::string value_;
};

inline std::ostream& operator<<(std::ostream& out,
                                const CharactersObject& obj) {
  return (out << obj.to_string());
}

} // lispp
