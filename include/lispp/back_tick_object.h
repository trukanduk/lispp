#pragma once

#include <lispp/object.h>
#include <lispp/object_ptr.h>

namespace lispp {

class BackTickObject : public DecoratorObject {
public:
  BackTickObject() = default;
  explicit BackTickObject(ObjectPtr<> value) : value_(value) {}
  ~BackTickObject() {}

  static std::string GetTypeName() {
    return "backtick";
  }

  ObjectPtr<> get_value() { return value_; }
  const ObjectPtr<> get_value() const { return value_; }
  void set_value(const ObjectPtr<>& value) { value_ = value; }

  BackTickObject* as_back_tick() override { return this; }
  const BackTickObject* as_back_tick() const override { return this; }

  bool operator==(const Object& other) const override;
  std::string to_string() const override;
  ObjectPtr<> eval(const std::shared_ptr<Scope>& scope) override;

private:
  static ObjectPtr<> process_object_on_eval(ObjectPtr<> object,
                                        const std::shared_ptr<Scope>& scope);

  ObjectPtr<> value_;
};

std::ostream& operator<<(std::ostream& out, const BackTickObject& obj);

} // lispp
