#include <lispp/scope.h>

#include <lispp/builtins.h>

namespace lispp {

Scope::Scope(const std::shared_ptr<Scope>& parent_scope)
    : parent_scope_(parent_scope) {}

bool Scope::has_value(const std::string& name) const {
  return (scope_.count(name) > 0) ||
         (parent_scope_ && parent_scope_->has_value(name));
}

ObjectPtr<> Scope::get_value(const std::string& name) {
  auto object_it = scope_.find(name);
  if (object_it != scope_.end()) {
    return object_it->second;
  } else if (parent_scope_) {
    return parent_scope_->get_value(name);
  } else {
    throw ScopeError("Cannot get '" + name + "'");
  }
}

void Scope::set_value(const std::string& name, const ObjectPtr<>& object) {
  scope_[name] = object;
}

void Scope::replace_value(const std::string& name, const ObjectPtr<>& object) {
  auto iter = scope_.find(name);
  if (iter != scope_.end()) {
    iter->second = object;
  } else if (has_parent_scope()) {
    parent_scope_->replace_value(name, object);
  } else {
    throw ScopeError("No variable named " + name);
  }
}

bool Scope::has_parent_scope() const {
  return bool(parent_scope_);
}

std::shared_ptr<Scope> Scope::get_parent_scope() {
  return parent_scope_;
}

std::shared_ptr<Scope> Scope::create_child_scope() {
  return std::make_shared<Scope>(shared_from_this());
}

} // lispp
