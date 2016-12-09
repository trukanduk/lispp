#pragma once

#include <unordered_map>
#include <memory>

#include <lispp/object.h>
#include <lispp/object_ptr.h>

namespace lispp {

class ScopeError : public std::runtime_error {
public:
  using runtime_error::runtime_error;
};

class Scope : public std::enable_shared_from_this<Scope> {
public:
  Scope() = default;
  explicit Scope(const std::shared_ptr<Scope>& parent_scope);

  bool has_value(const std::string& name) const;

  ObjectPtr<> get_value(const std::string& name);
  void set_value(const std::string& name, const ObjectPtr<>& object);

  void replace_value(const std::string& name, const ObjectPtr<>& object);

  bool has_parent_scope() const;
  std::shared_ptr<Scope> get_parent_scope();

  std::shared_ptr<Scope> create_child_scope();

protected:
  std::unordered_map<std::string, ObjectPtr<>> scope_;
  std::shared_ptr<Scope> parent_scope_;
};

} // lispp
