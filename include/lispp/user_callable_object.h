#pragma once

#include <vector>

#include <lispp/callable_object.h>

namespace lispp {

// FIXME: naming
class UserCallableObject : public CallableObject {
public:
  explicit UserCallableObject(const std::string& name,
                              const std::vector<std::string>& args,
                              const std::vector<ObjectPtr<>>& body,
                              const std::shared_ptr<Scope>& closure,
                              const std::string& rest_arg_name,
                              CallableType type = CallableType::kFunction);

protected:
  ObjectPtr<> execute_impl(const std::shared_ptr<Scope>& scope,
                           const std::vector<ObjectPtr<>>& args) override;

private:
  std::string name_;
  std::vector<std::string> args_;
  std::vector<ObjectPtr<>> body_;
  std::shared_ptr<Scope> closure_;
  std::string rest_arg_name_;
};

} // lispp
