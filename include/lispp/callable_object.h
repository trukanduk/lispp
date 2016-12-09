#pragma once

#include <vector>

#include <lispp/object.h>
#include <lispp/scope.h>

namespace lispp {

enum class CallableType {
  kFunction,
  kMacro
};

class CallableObject : public Object {
public:
  explicit CallableObject(CallableType type = CallableType::kFunction,
                          bool create_separate_scope = false)
      : type_(type), create_separate_scope_(create_separate_scope) {}

  static std::string GetTypeName() {
    return "callable";
  }

  CallableObject* as_callable() override { return this; }
  const CallableObject* as_callable() const override { return this; }

  std::string to_string() const override;
  CallableType get_type() const { return type_; }

  ObjectPtr<> eval(const std::shared_ptr<Scope>&) override { return this; }
  ObjectPtr<> execute(const std::shared_ptr<Scope>& scope,
                      const ObjectPtr<>& args);

protected:
  virtual ObjectPtr<> execute_impl(const std::shared_ptr<Scope>& scope,
                               const std::vector<ObjectPtr<>>& args) = 0;

private:
  ObjectPtr<> prepare_args(const ObjectPtr<>& src_args,
                           const std::shared_ptr<Scope>& scope) const;

  CallableType type_ = CallableType::kFunction;
  bool create_separate_scope_ = false;
};

std::ostream& operator<<(std::ostream& out, const CallableObject& obj);

} // lispp
