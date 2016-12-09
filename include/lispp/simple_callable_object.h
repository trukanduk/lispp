#pragma once

#include <functional>

#include <lispp/callable_object.h>

namespace lispp {

template<typename Callable>
class SimpleCallableObject : public CallableObject {
public:
  explicit SimpleCallableObject(
      Callable callable, CallableType type = CallableType::kFunction,
      bool create_separate_scope = false)
      : CallableObject(type, create_separate_scope), callable_(callable) {}

protected:
  ObjectPtr<> execute_impl(const std::shared_ptr<Scope>& scope,
                       const std::vector<ObjectPtr<>>& args) override {
    return callable_(scope, args);
  }

private:
  Callable callable_;
};

template<typename Callable>
ObjectPtr<SimpleCallableObject<Callable>> make_simple_callable(
    Callable callable, CallableType type = CallableType::kFunction,
    bool create_separate_scope = false) {
  return new SimpleCallableObject<Callable>(callable, type,
                                            create_separate_scope);
}

} // lispp
