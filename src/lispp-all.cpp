

#include <cassert>
#include <iostream>
#include <memory>
#include <string>

// file ../include/lispp/object.h
// #include <lispp/scope.h>


#include <type_traits>
#include <memory>

// file ../include/lispp/object_ptr.h
namespace lispp {

class Object;
class Scope;

template<typename ObjectType = Object>
class ObjectPtr final {
  template<typename OtherType>
  friend class ObjectPtr;

  struct ObjectPtrObjectChecker;
  // static_assert(std::is_base_of<Object, ObjectType>::value,
  //               "ObjectType template parameter must be inherited from Object");

public:
  ObjectPtr() = default;

  // non-explicit
  ObjectPtr(ObjectType* object);

  ObjectPtr(const ObjectPtr& other);

  template<typename SubType>
  ObjectPtr(const ObjectPtr<SubType>& other);

  ObjectPtr(ObjectPtr&& other);

  template<typename SubType>
  ObjectPtr(ObjectPtr<SubType>&& other);

  ObjectPtr& operator=(const ObjectPtr& other);

  template<typename SubType>
  ObjectPtr& operator=(const ObjectPtr<SubType>& other);

  ObjectPtr& operator=(ObjectPtr&& other);

  template<typename SubType>
  ObjectPtr& operator=(ObjectPtr<SubType>&& other);

  ~ObjectPtr();

  void reset();
  void reset(std::nullptr_t);

  template<typename SubType>
  void reset(SubType* object);

  ObjectType* release();

  ObjectType* get() const;
  bool valid() const;

  bool operator==(const ObjectPtr& other) const;
  bool operator==(const ObjectType* ptr) const;
  bool operator!=(const ObjectPtr& other) const;
  bool operator!=(const ObjectType* ptr) const;

  ObjectType& operator*() const;
  ObjectType* operator->() const;

  operator bool() const;
  bool operator!() const;

  void swap(ObjectPtr& other);

  bool safe_equal(const ObjectPtr& other) const;
  ObjectPtr<> safe_eval(const std::shared_ptr<Scope>& scope) const;
  template<typename OtherType>
  ObjectPtr<OtherType> safe_cast() const;

private:
  ObjectType* ptr_ = nullptr;
};

template<typename ObjectType>
bool operator==(const ObjectType* left,
                const ObjectPtr<ObjectType>& right);

template<typename ObjectType>
bool operator!=(const ObjectType* left,
                const ObjectPtr<ObjectType>& right);

template<typename ObjectType>
std::ostream& operator<<(std::ostream& out,
                         const ObjectPtr<ObjectType>& object_ptr) {
  out << "&" << object_ptr;
  return out;
}

} // lispp

// end file ../include/lispp/object_ptr.h


namespace lispp {

class Scope;

class ExecutionError : public std::runtime_error {
public:
  using runtime_error::runtime_error;
};

class BooleanObject;
class NumberObject;
class CharactersObject;
class SymbolObject;
class ConsObject;

class CallableObject;

class DecoratorObject;
class QuoteObject;
class CommaObject;
class BackTickObject;

// NOTE: Use intrusive_ptr instead of shared_ptr.
// It allows make some actions on ref-unref and
//   more powerful garbage collecting
class Object {
  template<typename T>
  friend class ObjectPtr;

public:
  Object() = default;
  Object(const Object&) = delete; // NOTE: ref counters?
  Object(Object&&) = delete; // NOTE: ref counters? Allow move?
  Object& operator=(const Object&) = delete; // NOTE: ref counters?
  Object& operator=(Object&&) = delete; // NOTE: ref counters? Allow move?
  virtual ~Object() = 0;

  static std::string GetTypeName();

  int get_ref_count() const;

  // NOTE: Fast casters
  virtual BooleanObject* as_boolean() { return nullptr; }
  virtual const BooleanObject* as_boolean() const { return nullptr; }
  virtual NumberObject* as_number() { return nullptr; }
  virtual const NumberObject* as_number() const { return nullptr; }
  virtual CharactersObject* as_characters() { return nullptr; }
  virtual const CharactersObject* as_characters() const { return nullptr; }
  virtual SymbolObject* as_symbol() { return nullptr; }
  virtual const SymbolObject* as_symbol() const { return nullptr; }
  virtual ConsObject* as_cons() { return nullptr; }
  virtual const ConsObject* as_cons() const { return nullptr; }
  virtual CallableObject* as_callable() { return nullptr; }
  virtual const CallableObject* as_callable() const { return nullptr; }
  virtual QuoteObject* as_quote() { return nullptr; }
  virtual const QuoteObject* as_quote() const { return nullptr; }
  virtual CommaObject* as_comma() { return nullptr; }
  virtual const CommaObject* as_comma() const { return nullptr; }
  virtual BackTickObject* as_back_tick() { return nullptr; }
  virtual const BackTickObject* as_back_tick() const { return nullptr; }

  template<typename ObjectType>
  ObjectType* as();

  template<typename ObjectType>
  const ObjectType* as() const;

  virtual bool operator==(const Object&) const { return false; }
  virtual bool operator!=(const Object& other) const { return *this != other; }

  virtual std::string to_string() const = 0;
  virtual ObjectPtr<> eval(const std::shared_ptr<Scope>& scope) = 0;

protected:
  int ref();
  int unref();
  int unref_nodelete();

private:
  int ref_count_ = 0;
};

std::ostream& operator<<(std::ostream& out, const Object& obj);

// NOTE: too lasy to create separated file...
class DecoratorObject : public Object {};

std::ostream& operator<<(std::ostream& out, const DecoratorObject& obj);

} // lispp



// file ../include/lispp/object-impl.h
namespace lispp {

template<typename ObjectType>
inline ObjectType* Object::as() {
  return dynamic_cast<ObjectType*>(this);
}

template<typename ObjectType>
inline const ObjectType* Object::as() const {
  return dynamic_cast<const ObjectType*>(this);
}

template<>
inline BooleanObject* Object::as<BooleanObject>() {
  return as_boolean();
}

template<>
inline const BooleanObject* Object::as<BooleanObject>() const {
  return as_boolean();
}

template<>
inline NumberObject* Object::as<NumberObject>() {
  return as_number();
}

template<>
inline const NumberObject* Object::as<NumberObject>() const {
  return as_number();
}

template<>
inline CharactersObject* Object::as<CharactersObject>() {
  return as_characters();
}

template<>
inline const CharactersObject* Object::as<CharactersObject>() const {
  return as_characters();
}

template<>
inline SymbolObject* Object::as<SymbolObject>() {
  return as_symbol();
}

template<>
inline const SymbolObject* Object::as<SymbolObject>() const {
  return as_symbol();
}

template<>
inline ConsObject* Object::as<ConsObject>() {
  return as_cons();
}

template<>
inline const ConsObject* Object::as<ConsObject>() const {
  return as_cons();
}

template<>
inline CallableObject* Object::as<CallableObject>() {
  return as_callable();
}

template<>
inline const CallableObject* Object::as<CallableObject>() const {
  return as_callable();
}

template<>
inline QuoteObject* Object::as<QuoteObject>() {
  return as_quote();
}

template<>
inline const QuoteObject* Object::as<QuoteObject>() const {
  return as_quote();
}

template<>
inline CommaObject* Object::as<CommaObject>() {
  return as_comma();
}

template<>
inline const CommaObject* Object::as<CommaObject>() const {
  return as_comma();
}

template<>
inline BackTickObject* Object::as<BackTickObject>() {
  return as_back_tick();
}

template<>
inline const BackTickObject* Object::as<BackTickObject>() const {
  return as_back_tick();
}

} // lispp
// end file ../include/lispp/object-impl.h



// file ../include/lispp/object_ptr-impl.h
namespace lispp {

template<typename ObjectType>
struct ObjectPtr<ObjectType>::ObjectPtrObjectChecker {
  static_assert(std::is_base_of<Object, ObjectType>::value,
                "ObjectType template parameter must be inherited from Object");
};

template<typename ObjectType>
ObjectPtr<ObjectType>::ObjectPtr(ObjectType* object) {
  ptr_ = object;
  if (ptr_ != nullptr) {
    ptr_->ref();
  }
}

// template<typename ObjectType>
// template<typename SubType>
// ObjectPtr<ObjectType>::ObjectPtr(SubType* object) {
//   ptr_ = object;
//   if (ptr_ != nullptr) {
//     ptr_->ref();
//   }
// }

template<typename ObjectType>
ObjectPtr<ObjectType>::ObjectPtr(const ObjectPtr<ObjectType>& other) {
  reset(other.ptr_);
}

template<typename ObjectType>
template<typename SubType>
ObjectPtr<ObjectType>::ObjectPtr(const ObjectPtr<SubType>& other) {
  reset(other.ptr_);
}

template<typename ObjectType>
ObjectPtr<ObjectType>::ObjectPtr(ObjectPtr<ObjectType>&& other) {
  ptr_  = other.ptr_;
  other.ptr_ = nullptr;
}

template<typename ObjectType>
template<typename SubType>
ObjectPtr<ObjectType>::ObjectPtr(ObjectPtr<SubType>&& other) {
  ptr_ = other.ptr_;
  other.ptr_ = nullptr;
}

template<typename ObjectType>
ObjectPtr<ObjectType>& ObjectPtr<ObjectType>::operator=(
    const ObjectPtr<ObjectType>& other) {
  reset(other.ptr_);
  return *this;
}

template<typename ObjectType>
template<typename SubType>
ObjectPtr<ObjectType>& ObjectPtr<ObjectType>::operator=(
    const ObjectPtr<SubType>& other) {
  reset(other.ptr_);
  return *this;
}

template<typename ObjectType>
ObjectPtr<ObjectType>& ObjectPtr<ObjectType>::operator=(
    ObjectPtr<ObjectType>&& other) {
  swap(other);
  other.reset();
  return *this;
}

template<typename ObjectType>
template<typename SubType>
ObjectPtr<ObjectType>& ObjectPtr<ObjectType>::operator=(
    ObjectPtr<SubType>&& other) {
  swap(other);
  other.reset();
  return *this;
}

template<typename ObjectType>
ObjectPtr<ObjectType>::~ObjectPtr() {
  reset();
}

template<typename ObjectType>
void ObjectPtr<ObjectType>::reset() {
  if (ptr_ != nullptr) {
    ptr_->unref();
  }
  ptr_ = nullptr;
}

template<typename ObjectType>
void ObjectPtr<ObjectType>::reset(std::nullptr_t) {
  reset();
}

template<typename ObjectType>
template<typename SubType>
void ObjectPtr<ObjectType>::reset(SubType* object) {
  reset();

  ptr_ = object;
  if (ptr_ != nullptr) {
    ptr_->ref();
  }
}

template<typename ObjectType>
ObjectType* ObjectPtr<ObjectType>::release() {
  const auto ptr = ptr_;
  if (ptr_) {
    ptr_->unref_nodelete();
  }
  ptr_ = nullptr;
  return ptr;
}

template<typename ObjectType>
ObjectType* ObjectPtr<ObjectType>::get() const {
  return ptr_;
}

template<typename ObjectType>
bool ObjectPtr<ObjectType>::valid() const {
  return (ptr_ != nullptr);
}

template<typename ObjectType>
bool ObjectPtr<ObjectType>::operator==(
    const ObjectPtr<ObjectType>& other) const {
  return (ptr_ == other.ptr_);
}

template<typename ObjectType>
bool ObjectPtr<ObjectType>::operator==(
    const ObjectType* ptr) const {
  return (ptr_ == ptr);
}

template<typename ObjectType>
bool ObjectPtr<ObjectType>::operator!=(
    const ObjectPtr<ObjectType>& other) const {
  return (ptr_ != other.ptr_);
}

template<typename ObjectType>
bool ObjectPtr<ObjectType>::operator!=(
    const ObjectType* ptr) const {
  return (ptr_ != ptr);
}

template<typename ObjectType>
ObjectType& ObjectPtr<ObjectType>::operator*() const {
  assert(ptr_ != nullptr);
  return *ptr_;
}

template<typename ObjectType>
ObjectType* ObjectPtr<ObjectType>::operator->() const {
  assert(ptr_ != nullptr);
  return ptr_;
}

template<typename ObjectType>
ObjectPtr<ObjectType>::operator bool() const {
  return valid();
}

template<typename ObjectType>
bool ObjectPtr<ObjectType>::operator!() const {
  return !valid();
}

template<typename ObjectType>
void ObjectPtr<ObjectType>::swap(ObjectPtr<ObjectType>& other) {
  std::swap(ptr_, other.ptr_);
}

template<typename ObjectType>
bool ObjectPtr<ObjectType>::safe_equal(
    const ObjectPtr<ObjectType>& other) const {
  return (!valid() && !other.valid()) ||
         (valid() && other.valid() && *ptr_ == *other.ptr_);
}

template<typename ObjectType>
ObjectPtr<> ObjectPtr<ObjectType>::safe_eval(
    const std::shared_ptr<Scope>& scope) const {
  if (!valid()) {
    throw ExecutionError("Cannot execute empty list!");
  }

  return ptr_->eval(scope);
  // return (ptr_ != nullptr ? ptr_->eval(scope) : nullptr);
}

template<typename ObjectType>
template<typename OtherType>
ObjectPtr<OtherType> ObjectPtr<ObjectType>::safe_cast() const {
  return (ptr_ != nullptr ? ptr_->template as<OtherType>() : nullptr);
}

template<typename ObjectType>
bool operator==(const ObjectType* left, const ObjectPtr<ObjectType>& right) {
  return (left == right.get());
}

template<typename ObjectType>
bool operator!=(const ObjectType* left, const ObjectPtr<ObjectType>& right) {
  return (left != right.get());
}

} // lispp
// end file ../include/lispp/object_ptr-impl.h

// end file ../include/lispp/object.h


// file ../include/lispp/back_tick_object.h
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
// end file ../include/lispp/back_tick_object.h


#include <functional>


#include <unordered_map>
#include <memory>


// file ../include/lispp/scope.h
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
// end file ../include/lispp/scope.h


#include <vector>


#include <sstream>


// file ../include/lispp/cons_object.h
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
// end file ../include/lispp/cons_object.h


// file ../include/lispp/list_utils.h
namespace lispp {

inline ObjectPtr<> make_list() {
  return nullptr;
}

inline ObjectPtr<> make_list(const ObjectPtr<>& object) {
  return object;
}

template<typename... Objects>
ObjectPtr<ConsObject> make_list(const ObjectPtr<>& head, Objects... tail) {
  ObjectPtr<ConsObject> result(new ConsObject);

  result->set_left_value(head);
  result->set_right_value(make_list(tail...));

  return result;
}

template<typename Callable>
ObjectPtr<> map_list(const ObjectPtr<>& head, Callable map_func) {
  if (!head.valid()) {
    return nullptr;
  } else if (head->as_cons() == nullptr) {
    return map_func(head);
  } else {
    ObjectPtr<ConsObject> cons_object(head->as_cons());

    ObjectPtr<ConsObject> result(new ConsObject);
    result->set_left_value(map_func(cons_object->get_left_value()));
    result->set_right_value(map_list(cons_object->get_right_value(), map_func));

    return result;
  }
}

std::vector<ObjectPtr<>> unpack_list(const ObjectPtr<>& lst);

ObjectPtr<> unpack_list_rest(const ObjectPtr<>& lst,
                        std::vector<ObjectPtr<>>* result);

constexpr int kInfiniteValuesNum = -1;

ObjectPtr<> unpack_list(const ObjectPtr<>& lst, int num_values,
                        std::vector<ObjectPtr<>>* result);

ObjectPtr<> pack_list(const std::vector<ObjectPtr<>>& lst,
                      const ObjectPtr<>& rest);

ObjectPtr<> pack_list(const std::vector<ObjectPtr<>>& lst);

} // lispp
// end file ../include/lispp/list_utils.h


// file ../src/core/back_tick_object.cpp
namespace lispp {

bool BackTickObject::operator==(const Object& other) const {
  const auto* other_back_tick = other.as_back_tick();
  return (other_back_tick != nullptr &&
          value_.safe_equal(other_back_tick->value_));
}

std::string BackTickObject::to_string() const {
  return "`" + (value_.valid() ? value_->to_string() : "nil");
}

ObjectPtr<> BackTickObject::eval(const std::shared_ptr<Scope>& scope) {
  return map_list(
      value_,
      std::bind(process_object_on_eval, std::placeholders::_1, scope));
}

ObjectPtr<> BackTickObject::process_object_on_eval(
    ObjectPtr<> object, const std::shared_ptr<Scope>& scope) {
  if (object == nullptr) {
    return nullptr;
  } else if (object->as_comma()) {
    return object->eval(scope);
  } else {
    return object;
  }
}

std::ostream& operator<<(std::ostream& out, const BackTickObject& obj) {
  return (out << obj.to_string());
}

} // lispp
// end file ../src/core/back_tick_object.cpp



#include <vector>


// file ../include/lispp/callable_object.h
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
// end file ../include/lispp/callable_object.h


#include <functional>


// file ../include/lispp/simple_callable_object.h
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
// end file ../include/lispp/simple_callable_object.h



// file ../include/lispp/builtins.h
// TODO: split this file into sections
namespace lispp {
namespace builtins {

// Basic macro
ObjectPtr<> cond_macro(const std::shared_ptr<Scope>& scope,
                       const std::vector<ObjectPtr<>>& args);

ObjectPtr<> if_macro(const std::shared_ptr<Scope>& scope,
                     const std::vector<ObjectPtr<>>& args);

ObjectPtr<> quote_macro(const std::shared_ptr<Scope>&,
                        const std::vector<ObjectPtr<>>& args);

ObjectPtr<> eval_macro(const std::shared_ptr<Scope>& scope,
                       const std::vector<ObjectPtr<>>& args);

ObjectPtr<> not_macro(const std::shared_ptr<Scope>& scope,
                      const std::vector<ObjectPtr<>>& args);

ObjectPtr<> or_macro(const std::shared_ptr<Scope>& scope,
                     const std::vector<ObjectPtr<>>& args);

ObjectPtr<> and_macro(const std::shared_ptr<Scope>& scope,
                      const std::vector<ObjectPtr<>>& args);

ObjectPtr<> let_macro(const std::shared_ptr<Scope>& scope,
                      const std::vector<ObjectPtr<>>& args);

// function & macro definning,
ObjectPtr<> lambda_macro(const std::shared_ptr<Scope>& scope,
                         const std::vector<ObjectPtr<>>& args);

ObjectPtr<> define_macro(const std::shared_ptr<Scope>& scope,
                         const std::vector<ObjectPtr<>>& args);

ObjectPtr<> defmacro_macro(const std::shared_ptr<Scope>& scope,
                           const std::vector<ObjectPtr<>>& args);

// setters
ObjectPtr<> set_macro(const std::shared_ptr<Scope>& scope,
                      const std::vector<ObjectPtr<>>& args);

ObjectPtr<> setcar_macro(const std::shared_ptr<Scope>& scope,
                         const std::vector<ObjectPtr<>>& args);

ObjectPtr<> setcdr_macro(const std::shared_ptr<Scope>& scope,
                         const std::vector<ObjectPtr<>>& args);

// list operations
ObjectPtr<> cons_function(const std::shared_ptr<Scope>&,
                          const std::vector<ObjectPtr<>>& args);

ObjectPtr<> car_function(const std::shared_ptr<Scope>&,
                         const std::vector<ObjectPtr<>>& args);

ObjectPtr<> cdr_function(const std::shared_ptr<Scope>&,
                         const std::vector<ObjectPtr<>>& args);

// type prediates
ObjectPtr<> nullp_function(const std::shared_ptr<Scope>&,
                           const std::vector<ObjectPtr<>>& args);

ObjectPtr<> numberp_function(const std::shared_ptr<Scope>&,
                             const std::vector<ObjectPtr<>>& args);

ObjectPtr<> booleanp_function(const std::shared_ptr<Scope>&,
                              const std::vector<ObjectPtr<>>& args);

ObjectPtr<> consp_function(const std::shared_ptr<Scope>&,
                           const std::vector<ObjectPtr<>>& args);

ObjectPtr<> listp_function(const std::shared_ptr<Scope>&,
                           const std::vector<ObjectPtr<>>& args);

ObjectPtr<> symbolp_function(const std::shared_ptr<Scope>&,
                             const std::vector<ObjectPtr<>>& args);

ObjectPtr<> stringp_function(const std::shared_ptr<Scope>&,
                             const std::vector<ObjectPtr<>>& args);

// number actions
ObjectPtr<> plus_function(const std::shared_ptr<Scope>&,
                          const std::vector<ObjectPtr<>>& args);

ObjectPtr<> minus_function(const std::shared_ptr<Scope>&,
                           const std::vector<ObjectPtr<>>& args);

ObjectPtr<> mul_function(const std::shared_ptr<Scope>&,
                         const std::vector<ObjectPtr<>>& args);

ObjectPtr<> div_function(const std::shared_ptr<Scope>&,
                         const std::vector<ObjectPtr<>>& args);

// misc functions
ObjectPtr<> string_len_function(const std::shared_ptr<Scope>&,
                                const std::vector<ObjectPtr<>>& args);

ObjectPtr<> print_function(const std::shared_ptr<Scope>&,
                           const std::vector<ObjectPtr<>>& args);

ObjectPtr<> exit_function(const std::shared_ptr<Scope>&,
                          const std::vector<ObjectPtr<>>& args);

ObjectPtr<> throw_function(const std::shared_ptr<Scope>&,
                           const std::vector<ObjectPtr<>>& args);
} // builtins

void init_global_scope(const std::shared_ptr<Scope>& scope);

void init_scope_with_builtins(const std::shared_ptr<Scope>& scope);
void init_scope_with_stdlibs(const std::shared_ptr<Scope>& scope);

} // lispp
// end file ../include/lispp/builtins.h


#include <functional>




// file ../include/lispp/boolean_object.h
namespace lispp {

class BooleanObject : public Object {
public:
  BooleanObject() = default;
  explicit BooleanObject(bool value) : value_(value) {}
  ~BooleanObject() {}

  static std::string GetTypeName() {
    return "boolean";
  }

  bool get_value() const { return value_; }
  void set_value(bool value) { value_ = value; }

  BooleanObject* as_boolean() override { return this; }
  const BooleanObject* as_boolean() const override { return this; }

  bool operator==(const Object& other) const override {
    const auto* other_identifier = other.as_boolean();
    return (other_identifier != nullptr && other_identifier->value_ == value_);
  }

  std::string to_string() const override {
    return (value_ ? "#t" : "#f");
  }

  ObjectPtr<> eval(const std::shared_ptr<Scope>&) override {
    return this;
  }

protected:
  bool value_ = false;
};

inline std::ostream& operator<<(std::ostream& out, const BooleanObject& obj) {
  return (out << obj.to_string());
}

} // lispp
// end file ../include/lispp/boolean_object.h


#include <string>


// file ../include/lispp/characters_object.h
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
// end file ../include/lispp/characters_object.h




// file ../include/lispp/comma_object.h
namespace lispp {

class CommaObject : public DecoratorObject {
public:
  CommaObject() = default;
  explicit CommaObject(ObjectPtr<> value) : value_(value) {}
  ~CommaObject() {}

  static std::string GetTypeName() {
    return "comma";
  }

  ObjectPtr<> get_value() { return value_; }
  const ObjectPtr<> get_value() const { return value_; }
  void set_value(const ObjectPtr<>& value) { value_ = value; }

  CommaObject* as_comma() override { return this; }
  const CommaObject* as_comma() const override { return this; }

  bool operator==(const Object& other) const override {
    const auto* other_comma = other.as_comma();
    return (other_comma != nullptr && value_.safe_equal(other_comma->value_));
  }

  std::string to_string() const override {
    return "," + (value_.valid() ? value_->to_string() : "nil");
  }

  ObjectPtr<> eval(const std::shared_ptr<Scope>& scope) override {
    return (value_.valid() ? value_->eval(scope) : nullptr);
  }

protected:
  ObjectPtr<> value_;
};

inline std::ostream& operator<<(std::ostream& out, const CommaObject& obj) {
  return (out << obj.to_string());
}

} // lispp
// end file ../include/lispp/comma_object.h



// file ../include/lispp/number_object.h
namespace lispp {

class NumberObject : public Object {
public:
  NumberObject() = default;
  explicit NumberObject(double value) : value_(value) {}
  ~NumberObject() {}

  static std::string GetTypeName() {
    return "number";
  }

  double get_value() const { return value_; }
  void set_value(double value) { value_ = value; }

  NumberObject* as_number() override { return this; }
  const NumberObject* as_number() const override { return this; }

  bool operator==(const Object& other) const override {
    const auto* other_number = other.as_number();
    return (other_number != nullptr && other_number->value_ == value_);
  }

  std::string to_string() const override {
    return std::to_string(value_);
  }

  ObjectPtr<> eval(const std::shared_ptr<Scope>&) override { return this; }

protected:
  double value_ = 0;
};

// TODO: define a macro?
inline std::ostream& operator<<(std::ostream& out, const NumberObject& obj) {
  return (out << obj.to_string());
}

} // lispp
// end file ../include/lispp/number_object.h



// file ../include/lispp/quote_object.h
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
// end file ../include/lispp/quote_object.h


#include <string>


// file ../include/lispp/symbol_object.h
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
// end file ../include/lispp/symbol_object.h


#include <vector>


// file ../include/lispp/user_callable_object.h
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
// end file ../include/lispp/user_callable_object.h

// end file ../include/lispp/objects_all.h


#include <sstream>


// file ../include/lispp/function_utils.h
namespace lispp {

bool is_true_value(const ObjectPtr<>& object);

bool is_true_condition(const ObjectPtr<>& condition,
                       const std::shared_ptr<Scope>& scope);

constexpr int kInvalidArgNumber = -1;

template<typename ObjectType>
void throw_bad_arg(const ObjectPtr<>& object,
                   const std::string& function_name,
                   int arg_number = kInvalidArgNumber) {
  std::stringstream ss;
  ss << function_name << ": expected " << ObjectType::GetTypeName();
  if (arg_number != kInvalidArgNumber) {
    ss << " for arg " << (arg_number + 1);
  }
  ss << " got ";
  if (object == nullptr) {
    ss << "nil";
  } else {
    ss << *object;
  }

  throw ExecutionError(ss.str());
}

template<typename ObjectType>
ObjectPtr<ObjectType> arg_cast(const ObjectPtr<>& object,
                               const std::string& function_name,
                               int arg_number = kInvalidArgNumber) {
  auto result = object.safe_cast<ObjectType>();

  if (!result.valid()) {
    throw_bad_arg<ObjectType>(object, function_name, arg_number);
  }

  return result;
}

void check_args_count(const std::string& function_name,
                      std::size_t args_count,
                      std::size_t expected_args_count,
                      CallableType type = CallableType::kFunction);

constexpr std::size_t kInfiniteArgs = ~0U;

void check_args_count(const std::string& function_name,
                      std::size_t args_count,
                      std::size_t expected_args_count_min,
                      std::size_t expected_args_count_max,
                      CallableType type = CallableType::kFunction);

} // lispp
// end file ../include/lispp/function_utils.h




#include <memory>


#include <iostream>
#include <string>


#include <iostream>
#include <string>

// file ../include/lispp/token.h
namespace lispp {

enum class TokenType {
  kNumber,       // +123 or -.4
  kCharacters,   // "foo"
  kSymbol,       // foo-bar
  kComma,        // ,
  kBackTick,     // `
  // kNumQuote,     // #' -- Pure lip only!?
  kDot,          // .
  kQuote,        // '
  kOpenBracket,  // (
  kCloseBracket, // )
  kEndLine,      // \n
  kEnd,          // <end of file>

  kUndefined
};

struct Token final {
  Token() = default;
  explicit Token(TokenType type);
  Token(TokenType type, double number_value);
  Token(TokenType type, const std::string& string_value);

  bool operator==(const Token& other) const;
  bool operator!=(const Token& other) const;

  TokenType type = TokenType::kUndefined;

  bool bool_value = false;
  double number_value = 0.0;
  std::string string_value;
};

std::ostream& operator<<(std::ostream& out, const Token& token);

} // lispp
// end file ../include/lispp/token.h


// file ../include/lispp/tokenizer.h
namespace lispp {

class TokenizerError : public std::runtime_error {
public:
  using runtime_error::runtime_error;
};

class ITokenizer {
public:
  virtual ~ITokenizer() = 0;

  virtual Token next_token() = 0;
  virtual Token current_token() = 0;
  virtual Token peek_token() = 0;
  virtual bool has_more_tokens() = 0;

  virtual int get_current_line() = 0;
  virtual void clear() = 0;
};

} // lispp
// end file ../include/lispp/tokenizer.h


// file ../include/lispp/parser.h
namespace lispp {

class ParserError : public std::runtime_error {
public:
  using runtime_error::runtime_error;
};

class Parser {
public:
  explicit Parser(ITokenizer* tokenizer);

  bool has_objects(bool ignore_endlines = true);
  ObjectPtr<> parse_object();

private:
  ObjectPtr<ConsObject> parse_begun_list();
  void expect_token(const Token& tok, bool extract = true);
  void skip_endlines();

  ITokenizer* tokenizer_;
};

} // lispp
// end file ../include/lispp/parser.h


// file ../include/lispp/virtual_machine_base.h
namespace lispp {

class VirtualMachineBase {
public:
  ObjectPtr<> parse();
  ObjectPtr<> eval();
  ObjectPtr<> eval_all();

  Parser& get_parser();
  std::shared_ptr<Scope> get_global_scope();

protected:
  explicit VirtualMachineBase(ITokenizer* tokenizer);
  VirtualMachineBase(const std::shared_ptr<Scope>& global_scope,
                     ITokenizer* tokenizer);

  void init(ITokenizer* tokenizer);

  ITokenizer* get_tokenizer_base();

private:
  ITokenizer* tokenizer_;
  std::unique_ptr<Parser> parser_;
  std::shared_ptr<Scope> global_scope_;
};

} // lispp
// end file ../include/lispp/virtual_machine_base.h


#include <sstream>


#include <iostream>
#include <string>
#include <unordered_set>
#include <cctype>


// file ../include/lispp/istream_tokenizer.h
namespace lispp {

// TODO: file_tokenizer
class IstreamTokenizer : public ITokenizer {
public:
  IstreamTokenizer() = default;
  explicit IstreamTokenizer(std::istream& input);
  ~IstreamTokenizer() override;

  Token next_token() override;
  Token current_token() override;
  Token peek_token() override;
  bool has_more_tokens() override;

  int get_current_line() override;
  void clear() override;

protected:
  // NOTE: call this from subclasses in constructors.
  void set_input_stream(std::istream& input);
  void check_input_stream();

private:
  Token parse_token();
  Token parse_characters_token();
  bool is_symbol_token_start(char current_char);
  Token parse_symbol_token();
  Token parse_number_token();
  Token parse_one_symbol_token();

  template<typename Condition>
  std::string read_while(Condition condition,
                        const std::string& error_message) {
    std::string result;

    while (input_->good() && condition(input_->peek())) {
      result.push_back(input_->get());
    }

    if (!input_->good() && !error_message.empty()) {
      throw TokenizerError(error_message);
    }

    return result;
  }

  template<typename Condition>
  std::string read_while(Condition condition) {
    return read_while(condition, "");
  }

  static bool IsWhiteSpace(char c, bool accept_eol = false) {
    return std::isspace(c) && (accept_eol || c != '\n');
  }

  static bool IsDigit(char c) {
    return std::isdigit(c);
  }

  static bool IsSign(char c) {
    return (c == '-') || (c == '+');
  }

  static bool IsDigitExt(char c) {
    return IsDigit(c) || IsSign(c) || (c == '.');
  }

  static bool IsNotQuotes(char c) {
    return c != '"';
  }

  static bool IsInitialOfSymbol(char c) {
    static const std::unordered_set<char> kValidSymbols{
      '!', '$', '%', '&', '*', '/', ':', '<', '=', '>', '?', '~', '_', '^', '#'
    };

    return std::isalpha(c) || (kValidSymbols.count(c) > 0);
  }

  static bool IsSymbolChar(char c) {
    return IsInitialOfSymbol(c) || std::isdigit(c) ||
           c == '.' || c == '-' || c == '+';
  }

  void skip_whitespaces(bool with_eol = false);
  void expect_char(char c, bool extract = true);
  int peek_next_char();


  std::istream* input_ = nullptr;
  Token next_token_;
  Token current_token_;
  int current_line_ = 0;
};

} // lispp
// end file ../include/lispp/istream_tokenizer.h


// file ../include/lispp/string_tokenizer.h
namespace lispp {

class StringTokenizer : public IstreamTokenizer {
public:
  StringTokenizer();
  StringTokenizer(const std::string& initial_value);

  void append(const std::string& initial_value);
  void clear() override;

  StringTokenizer& operator<<(const std::string& value) {
    append(value);
    return *this;
  }

  template<typename T>
  StringTokenizer& operator<<(const T& value) {
    // FIXME: OPTIMIZE:
    std::stringstream ss;
    ss << value;
    append(ss.str());
    return *this;
  }

private:
  std::stringstream input_ss_;
};

} // lispp
// end file ../include/lispp/string_tokenizer.h


// file ../include/lispp/virtual_machine.h
namespace lispp {

template<typename TokenizerType = StringTokenizer>
class VirtualMachine : public VirtualMachineBase {
public:
  template<typename... TokenizerArgs>
  VirtualMachine(const std::shared_ptr<Scope>& global_scope,
                 const TokenizerArgs&... tokenizer_args)
      : VirtualMachineBase(global_scope, nullptr) {
    tokenizer_.reset(new TokenizerType(tokenizer_args...));
    init(tokenizer_.get());
  }

  template<typename... TokenizerArgs>
  VirtualMachine(TokenizerArgs&... tokenizer_args)
      : VirtualMachineBase(nullptr) {
    tokenizer_.reset(new TokenizerType(tokenizer_args...));
    init(tokenizer_.get());
  }

  TokenizerType& get_tokenizer() { return *tokenizer_; }

private:
  std::unique_ptr<TokenizerType> tokenizer_;
};

template<>
class VirtualMachine<StringTokenizer> : public VirtualMachineBase {
public:
  VirtualMachine(const std::shared_ptr<Scope>& global_scope,
                 const std::string& initial_code = "")
      : VirtualMachineBase(global_scope, nullptr) {
    tokenizer_.reset(new StringTokenizer(initial_code));
    init(tokenizer_.get());
  }

  VirtualMachine(const std::string& initial_code = "")
      : VirtualMachineBase(nullptr) {
    tokenizer_.reset(new StringTokenizer(initial_code));
    init(tokenizer_.get());
  }

  using VirtualMachineBase::parse;

  ObjectPtr<> parse(const std::string& code) {
    get_tokenizer().clear();
    get_tokenizer() << code;
    return parse();
  }

  using VirtualMachineBase::eval;

  ObjectPtr<> eval(const std::string& code) {
    get_tokenizer().clear();
    get_tokenizer() << code;
    return eval();
  }

  using VirtualMachineBase::eval_all;

  ObjectPtr<> eval_all(const std::string& code) {
    get_tokenizer().clear();
    get_tokenizer() << code;
    return eval_all();
  }

  StringTokenizer& get_tokenizer() { return *tokenizer_; }

private:
  std::unique_ptr<StringTokenizer> tokenizer_;
};

}
// end file ../include/lispp/virtual_machine.h


// file ../src/core/builtins.cpp
// FIXME: how to split this code to several files?
namespace lispp {
namespace builtins {

ObjectPtr<> cond_macro(const std::shared_ptr<Scope>& scope,
                       const std::vector<ObjectPtr<>>& args) {
  for (auto& branch : args) {
    auto cons_branch = arg_cast<ConsObject>(branch, "cond");

    auto unpacked_branch = unpack_list(cons_branch);
    ObjectPtr<> condition = unpacked_branch[0];
    ObjectPtr<> action = unpacked_branch[1];

    if (is_true_condition(condition, scope)) {
      return action->eval(scope);
    }
  }

  return nullptr;
}

ObjectPtr<> if_macro(const std::shared_ptr<Scope>& scope,
                     const std::vector<ObjectPtr<>>& args) {
  check_args_count("if", args.size(), 2, 3, CallableType::kMacro);

  ObjectPtr<> condition = args[0];
  if (is_true_condition(condition, scope)) {
    return args[1].safe_eval(scope);
  } else if (args.size() == 3) {
    return args[2].safe_eval(scope);
  } else {
    return nullptr;
  }
}

ObjectPtr<> quote_macro(const std::shared_ptr<Scope>&,
                    const std::vector<ObjectPtr<>>& args) {
  check_args_count("quote", args.size(), 1, CallableType::kMacro);

  return args[0];
}

ObjectPtr<> eval_macro(const std::shared_ptr<Scope>& scope,
                   const std::vector<ObjectPtr<>>& args) {
  check_args_count("eval", args.size(), 1, CallableType::kMacro);

  return args[0].safe_eval(scope).safe_eval(scope);
}

ObjectPtr<> not_macro(const std::shared_ptr<Scope>& scope,
                  const std::vector<ObjectPtr<>>& args) {
  check_args_count("not", args.size(), 1, CallableType::kMacro);

  bool condition_value = is_true_condition(args[0], scope);
  return new BooleanObject(!condition_value);
}

ObjectPtr<> or_macro(const std::shared_ptr<Scope>& scope,
                 const std::vector<ObjectPtr<>>& args) {
  ObjectPtr<> result(new BooleanObject(false));
  for (std::size_t arg_index = 0;
       arg_index < args.size() && !is_true_value(result);
       ++arg_index) {
    result = args[arg_index].safe_eval(scope);
  }

  return result;
}

ObjectPtr<> and_macro(const std::shared_ptr<Scope>& scope,
                  const std::vector<ObjectPtr<>>& args) {
  ObjectPtr<> result(new BooleanObject(true));
  for (std::size_t arg_index = 0;
       arg_index < args.size() && is_true_value(result);
       ++arg_index) {
    result = args[arg_index].safe_eval(scope);
  }

  return result;
}

ObjectPtr<> let_macro(const std::shared_ptr<Scope>& scope,
                  const std::vector<ObjectPtr<>>& args) {
  check_args_count("let", args.size(), 1, kInfiniteArgs, CallableType::kMacro);

  std::shared_ptr<Scope> local_scope = std::make_shared<Scope>(scope);
  auto varlist = unpack_list(args[0]);
  for (std::size_t var_index = 0; var_index < varlist.size(); ++var_index) {
    auto var_info = unpack_list(varlist[var_index]);
    if (var_info.size() != 2) {
      throw ExecutionError("let macro expects exactly 2 values"
                           "for variable item");
    }

    auto varname = arg_cast<SymbolObject>(var_info[0], "let");
    ObjectPtr<> eval_result = var_info[1].safe_eval(scope);

    local_scope->set_value(varname->get_value(), eval_result);
  }

  ObjectPtr<> result;
  for (std::size_t expr_index = 1; expr_index < args.size(); ++expr_index) {
    result = args[expr_index].safe_eval(local_scope);
  }

  return result;
}

namespace {
  void parse_callable_definition(const std::string& macro_name,
                                 ObjectPtr<> cons_args,
                                 std::vector<std::string>* arg_names,
                                 std::string* rest_arg_name) {
    std::vector<ObjectPtr<>> arg_symbols;
    auto rest_arg = unpack_list_rest(cons_args, &arg_symbols);
    arg_names->reserve(arg_symbols.size());
    for (std::size_t arg_index = 0; arg_index < arg_symbols.size(); ++arg_index) {
      auto sym_arg = arg_cast<SymbolObject>(arg_symbols[arg_index], macro_name);
      arg_names->push_back(sym_arg->get_value());
    }

    if (rest_arg.valid()) {
      auto rest_arg_symbol = arg_cast<SymbolObject>(rest_arg, macro_name);
      *rest_arg_name = rest_arg_symbol->get_value();
    } else {
      *rest_arg_name = "";
    }
  }
} // namespace

ObjectPtr<> lambda_macro(const std::shared_ptr<Scope>& scope,
                         const std::vector<ObjectPtr<>>& args) {
  check_args_count("lambda", args.size(), 2, kInfiniteArgs,
                   CallableType::kMacro);

  std::vector<std::string> arg_names;
  std::string rest_arg_name;
  parse_callable_definition("lambda", args[0], &arg_names, &rest_arg_name);
  std::vector<ObjectPtr<>> body(std::next(args.begin()), args.end());

  static int lambda_number = 0;
  std::stringstream name_ss;
  name_ss << "<lambda#" << lambda_number++ << ">";

  return new UserCallableObject(name_ss.str(), arg_names, body,
                                scope, rest_arg_name);
}

namespace {

  ObjectPtr<> define_callable(const std::string& macro_name,
                              const std::shared_ptr<Scope>& scope,
                              const std::vector<ObjectPtr<>>& args,
                              CallableType callable_type) {
    auto header = arg_cast<ConsObject>(args[0], macro_name);

    auto function_name = arg_cast<SymbolObject>(
        header->get_left_value(), macro_name);
    auto function_args = header->get_right_value();

    std::vector<std::string> arg_names;
    std::string rest_arg_name;
    parse_callable_definition(macro_name, function_args,
                              &arg_names, &rest_arg_name);

    std::vector<ObjectPtr<>> body(std::next(args.begin()), args.end());

    ObjectPtr<CallableObject> result(new UserCallableObject(
        function_name->get_value(), arg_names, body, scope,
        rest_arg_name, callable_type));

    scope->set_value(function_name->get_value(), result);
    return result;
  }

} // namespace

ObjectPtr<> define_macro(const std::shared_ptr<Scope>& scope,
                     const std::vector<ObjectPtr<>>& args) {
  check_args_count("define", args.size(), 2, kInfiniteArgs,
                   CallableType::kMacro);

  auto varname_symbol = args[0].safe_cast<SymbolObject>();
  if (varname_symbol.valid()) {
    check_args_count("define", args.size(), 2, CallableType::kMacro);

    auto result = args[1].safe_eval(scope);
    scope->set_value(varname_symbol->get_value(), result);
    return result;
  } else {
    return define_callable("define", scope, args, CallableType::kFunction);
  }
}

ObjectPtr<> defmacro_macro(const std::shared_ptr<Scope>& scope,
                       const std::vector<ObjectPtr<>>& args) {
  check_args_count("define-macro", args.size(), 1, kInfiniteArgs,
                   CallableType::kMacro);

  define_callable("define-macro", scope, args, CallableType::kMacro);
  return nullptr;
}

ObjectPtr<> set_macro(const std::shared_ptr<Scope>& scope,
                  const std::vector<ObjectPtr<>>& args) {
  check_args_count("set!", args.size(), 2, CallableType::kMacro);

  auto sym_name = arg_cast<SymbolObject>(args[0], "set!");

  auto eval_result = args[1].safe_eval(scope);
  scope->replace_value(sym_name->get_value(), eval_result);

  return nullptr;
}

ObjectPtr<> setcar_macro(const std::shared_ptr<Scope>& scope,
                         const std::vector<ObjectPtr<>>& args) {
  check_args_count("set-car!", args.size(), 2, CallableType::kMacro);

  auto sym_name = arg_cast<SymbolObject>(args[0], "set-car!");

  auto object = scope->get_value(sym_name->get_value()).safe_cast<ConsObject>();
  if (!object.valid()) {
    throw ExecutionError("Variable of set-car! must be a cons");
  }

  object->set_left_value(args[1].safe_eval(scope));

  return object;
}

ObjectPtr<> setcdr_macro(const std::shared_ptr<Scope>& scope,
                         const std::vector<ObjectPtr<>>& args) {
  check_args_count("set-cdr!", args.size(), 2, CallableType::kMacro);

  auto sym_name = arg_cast<SymbolObject>(args[0], "set-cdr!");

  auto object = scope->get_value(sym_name->get_value()).safe_cast<ConsObject>();
  if (!object.valid()) {
    throw ExecutionError("Variable of set-cdr! must be a cons");
  }

  object->set_right_value(args[1].safe_eval(scope));

  return object;
}

ObjectPtr<> cons_function(const std::shared_ptr<Scope>&,
                          const std::vector<ObjectPtr<>>& args) {
  check_args_count("cons", args.size(), 2, CallableType::kMacro);

  ObjectPtr<> result(new ConsObject(args[0], args[1]));
  return result;
}

ObjectPtr<> car_function(const std::shared_ptr<Scope>&,
                         const std::vector<ObjectPtr<>>& args) {
  check_args_count("car", args.size(), 1, CallableType::kMacro);

  auto cons = arg_cast<ConsObject>(args[0], "car", 0);
  return cons->get_left_value();
}

ObjectPtr<> cdr_function(const std::shared_ptr<Scope>&,
                         const std::vector<ObjectPtr<>>& args) {
  check_args_count("cdr", args.size(), 1, CallableType::kMacro);

  auto cons = arg_cast<ConsObject>(args[0], "cdr", 0);
  return cons->get_right_value();
}

ObjectPtr<> nullp_function(const std::shared_ptr<Scope>&,
                           const std::vector<ObjectPtr<>>& args) {
  check_args_count("null?", args.size(), 1);

  return new BooleanObject(!args[0].valid());
}

ObjectPtr<> numberp_function(const std::shared_ptr<Scope>&,
                             const std::vector<ObjectPtr<>>& args) {
  check_args_count("number?", args.size(), 1);

  bool result_value = args[0].safe_cast<NumberObject>().valid();
  return new BooleanObject(result_value);
}

ObjectPtr<> booleanp_function(const std::shared_ptr<Scope>&,
                              const std::vector<ObjectPtr<>>& args) {
  check_args_count("boolean?", args.size(), 1);

  bool result_value = args[0].safe_cast<BooleanObject>().valid();
  return new BooleanObject(result_value);
}

ObjectPtr<> consp_function(const std::shared_ptr<Scope>&,
                           const std::vector<ObjectPtr<>>& args) {
  check_args_count("cons?", args.size(), 1);

  bool result_value = args[0].safe_cast<ConsObject>().valid();
  return new BooleanObject(result_value);
}

ObjectPtr<> listp_function(const std::shared_ptr<Scope>&,
                           const std::vector<ObjectPtr<>>& args) {
  check_args_count("list?", args.size(), 1);

  bool result = true;
  ObjectPtr<> tail = args[0];
  while (result && tail.valid()) {
    auto cons_tail = tail.safe_cast<ConsObject>();
    if (cons_tail.valid()) {
      tail = cons_tail->get_right_value();
    } else {
      result = false;
    }
  }
  return new BooleanObject(result);
}

ObjectPtr<> symbolp_function(const std::shared_ptr<Scope>&,
                             const std::vector<ObjectPtr<>>& args) {
  check_args_count("symbol?", args.size(), 1);

  bool result_value = args[0].safe_cast<SymbolObject>().valid();
  return new BooleanObject(result_value);
}

ObjectPtr<> stringp_function(const std::shared_ptr<Scope>&,
                             const std::vector<ObjectPtr<>>& args) {
  check_args_count("string?", args.size(), 1);

  bool result_value = args[0].safe_cast<CharactersObject>().valid();
  return new BooleanObject(result_value);
}

ObjectPtr<> plus_function(const std::shared_ptr<Scope>&,
                          const std::vector<ObjectPtr<>>& args) {
  double result = 0;
  for (std::size_t arg_index = 0; arg_index < args.size(); ++arg_index) {
    auto number = arg_cast<NumberObject>(args[arg_index], "+", arg_index);

    result += number->get_value();
  }

  return new NumberObject(result);
}

ObjectPtr<> minus_function(const std::shared_ptr<Scope>&,
                           const std::vector<ObjectPtr<>>& args) {
  if (args.empty()) {
    throw ExecutionError("- requires at least one argument");
  }

  auto first_number = arg_cast<NumberObject>(args[0], "-", 0);
  double result = first_number->get_value();

  if (args.size() == 1) {
    return new NumberObject(-result);
  }

  for (std::size_t arg_index = 1; arg_index < args.size(); ++arg_index) {
    auto number = arg_cast<NumberObject>(args[arg_index], "-", arg_index);

    result -= number->get_value();
  }

  return new NumberObject(result);
}

ObjectPtr<> mul_function(const std::shared_ptr<Scope>&,
                         const std::vector<ObjectPtr<>>& args) {
  double result = 1;
  for (std::size_t arg_index = 0; arg_index < args.size(); ++arg_index) {
    auto number = arg_cast<NumberObject>(args[arg_index], "*", arg_index);

    result *= number->get_value();
  }

  return new NumberObject(result);
}

ObjectPtr<> div_function(const std::shared_ptr<Scope>&,
                         const std::vector<ObjectPtr<>>& args) {
  if (args.empty()) {
    throw ExecutionError("/ requires at least one argument");
  }

  auto first_number = arg_cast<NumberObject>(args[0], "/", 0);
  double result = first_number->get_value();

  if (args.size() == 1) {
    return new NumberObject(1 / result);
  }

  for (std::size_t arg_index = 1; arg_index < args.size(); ++arg_index) {
    auto number = arg_cast<NumberObject>(args[arg_index], "/", arg_index);

    result /= number->get_value();
  }

  return new NumberObject(result);
}

// FIXME: move to header?
template<typename ObjectType, typename Comparator>
struct CompareFunc {
  CompareFunc(const std::string& comp_name)
      : comp_name(comp_name) {}

  ObjectPtr<> operator()(const std::shared_ptr<Scope>&,
                         const std::vector<ObjectPtr<>>& args) const {
    if (args.empty()) {
      return new BooleanObject(true);
    }

    check_args_count(comp_name, args.size(), 2, kInfiniteArgs);

    Comparator comparator;

    bool result = true;
    auto last_obj = arg_cast<ObjectType>(args[0], comp_name, 0);
    for (std::size_t arg_index = 1;
         arg_index < args.size() && result;
         ++arg_index) {
      auto curr_obj = arg_cast<ObjectType>(args[arg_index],
                                           comp_name, arg_index);

      result = result && comparator(last_obj->get_value(),
                                    curr_obj->get_value());
      last_obj = curr_obj;
    }

    return new BooleanObject(result);
  }

  std::string comp_name;
};

ObjectPtr<> string_len_function(const std::shared_ptr<Scope>&,
                                const std::vector<ObjectPtr<>>& args) {
  check_args_count("string-length", args.size(), 1);
  auto string = arg_cast<CharactersObject>(args[0], "string-length");

  return new NumberObject(string->get_value().size());
}

ObjectPtr<> print_function(const std::shared_ptr<Scope>&,
                           const std::vector<ObjectPtr<>>& args) {
  for (auto& object : args) {
    if (object.valid()) {
      std::cout << *object;
    } else {
      std::cout << "nil";
    }
    std::cout << std::endl;
  }

  return nullptr;
}

ObjectPtr<> exit_function(const std::shared_ptr<Scope>&,
                          const std::vector<ObjectPtr<>>& args) {
  int code = 0;
  if (args.size() > 0) {
    auto number = args[0].safe_cast<NumberObject>();

    if (number.valid()) {
      code = static_cast<int>(number->get_value());
    }
  }

  exit(code);
  return nullptr;
}

ObjectPtr<> throw_function(const std::shared_ptr<Scope>&,
                           const std::vector<ObjectPtr<>>& args) {
  std::stringstream ss;
  ss << "Throw from code: ";
  for (auto& arg : args) {
    ss << (arg.valid() ? arg->to_string() : "nil") << " ";
  }

  throw ExecutionError(ss.str());
}

extern const char* kBuiltinsStdlib_common;

} // builtins

void init_global_scope(const std::shared_ptr<Scope>& scope) {
  init_scope_with_builtins(scope);
  init_scope_with_stdlibs(scope);
}

void init_scope_with_builtins(const std::shared_ptr<Scope>& scope) {
  using namespace builtins;

  // Built-in macro
  static ObjectPtr<CallableObject> cond(
      make_simple_callable(cond_macro, CallableType::kMacro));
  scope->set_value("cond", cond);

  static ObjectPtr<CallableObject> if_(
      make_simple_callable(if_macro, CallableType::kMacro));
  scope->set_value("if", if_);

  static ObjectPtr<CallableObject> quote(
      make_simple_callable(quote_macro, CallableType::kMacro));
  scope->set_value("quote", quote);

  static ObjectPtr<CallableObject> eval(
      make_simple_callable(eval_macro, CallableType::kMacro));
  scope->set_value("eval", eval);

  static ObjectPtr<CallableObject> let(
      make_simple_callable(let_macro, CallableType::kMacro, true));
  scope->set_value("let", let);

  static ObjectPtr<CallableObject> lambda(
      make_simple_callable(lambda_macro, CallableType::kMacro));
  scope->set_value("lambda", lambda);

  static ObjectPtr<CallableObject> define(
      make_simple_callable(define_macro, CallableType::kMacro));
  scope->set_value("define", define);

  static ObjectPtr<CallableObject> defmacro(
      make_simple_callable(defmacro_macro, CallableType::kMacro));
  scope->set_value("define-macro", defmacro);

  static ObjectPtr<CallableObject> set_(
      make_simple_callable(set_macro, CallableType::kMacro));
  scope->set_value("set!", set_);

  static ObjectPtr<CallableObject> setcar(
      make_simple_callable(setcar_macro, CallableType::kMacro));
  scope->set_value("set-car!", setcar);

  static ObjectPtr<CallableObject> setcdr(
      make_simple_callable(setcdr_macro, CallableType::kMacro));
  scope->set_value("set-cdr!", setcdr);

  // Boolean macroses
  static ObjectPtr<CallableObject> not_(
      make_simple_callable(not_macro, CallableType::kMacro));
  scope->set_value("not", not_);

  static ObjectPtr<CallableObject> or_(
      make_simple_callable(or_macro, CallableType::kMacro));
  scope->set_value("or", or_);

  static ObjectPtr<CallableObject> and_(
      make_simple_callable(and_macro, CallableType::kMacro));
  scope->set_value("and", and_);

  // List operators
  static ObjectPtr<CallableObject> cons(make_simple_callable(cons_function));
  scope->set_value("cons", cons);

  static ObjectPtr<CallableObject> car(make_simple_callable(car_function));
  scope->set_value("car", car);

  static ObjectPtr<CallableObject> cdr(make_simple_callable(cdr_function));
  scope->set_value("cdr", cdr);

  // Built-in predicates
  static ObjectPtr<CallableObject> nullp(make_simple_callable(nullp_function));
  scope->set_value("null?", nullp);

  static ObjectPtr<CallableObject> numberp(
      make_simple_callable(numberp_function));
  scope->set_value("number?", numberp);

  static ObjectPtr<CallableObject> booleanp(
      make_simple_callable(booleanp_function));
  scope->set_value("boolean?", booleanp);

  static ObjectPtr<CallableObject> consp(make_simple_callable(consp_function));
  scope->set_value("cons?", consp);

  static ObjectPtr<CallableObject> listp(make_simple_callable(listp_function));
  scope->set_value("list?", listp);

  static ObjectPtr<CallableObject> symbolp(
      make_simple_callable(symbolp_function));
  scope->set_value("symbol?", symbolp);

  static ObjectPtr<CallableObject> stringp(
      make_simple_callable(stringp_function));
  scope->set_value("string?", stringp);

  // Number operators
  static ObjectPtr<CallableObject> plus(make_simple_callable(plus_function));
  scope->set_value("+", plus);

  static ObjectPtr<CallableObject> minus(make_simple_callable(minus_function));
  scope->set_value("-", minus);

  static ObjectPtr<CallableObject> mul(make_simple_callable(mul_function));
  scope->set_value("*", mul);

  static ObjectPtr<CallableObject> div(make_simple_callable(div_function));
  scope->set_value("/", div);

  static ObjectPtr<CallableObject> less(
      make_simple_callable(CompareFunc<NumberObject,
                                       std::less<double>>("<")));
  scope->set_value("<", less);

  static ObjectPtr<CallableObject> less_equal(
      make_simple_callable(CompareFunc<NumberObject,
                                       std::less_equal<double>>("<=")));
  scope->set_value("<=", less_equal);

  static ObjectPtr<CallableObject> greater(
      make_simple_callable(CompareFunc<NumberObject,
                                       std::greater<double>>(">")));
  scope->set_value(">", greater);

  static ObjectPtr<CallableObject> greater_equal(
      make_simple_callable(CompareFunc<NumberObject,
                                       std::greater_equal<double>>(">=")));
  scope->set_value(">=", greater_equal);

  static ObjectPtr<CallableObject> equal(
      make_simple_callable(CompareFunc<NumberObject,
                                       std::equal_to<double>>("=")));
  scope->set_value("=", equal);

  // Characters operations
  static ObjectPtr<CallableObject> string_len(
      make_simple_callable(string_len_function));
  scope->set_value("string-length", string_len);

  static ObjectPtr<CallableObject> less_chars(
      make_simple_callable(
          CompareFunc<CharactersObject, std::less<std::string>>("string<?")));
  scope->set_value("string<?", less_chars);

  static ObjectPtr<CallableObject> less_equal_chars(
      make_simple_callable(
          CompareFunc<CharactersObject,
                      std::less_equal<std::string>>("string<=?")));
  scope->set_value("string<=?", less_equal_chars);

  static ObjectPtr<CallableObject> greater_chars(
      make_simple_callable(
          CompareFunc<CharactersObject,
                      std::greater<std::string>>("string>?")));
  scope->set_value("string>?", greater_chars);

  static ObjectPtr<CallableObject> greater_equal_chars(
      make_simple_callable(
          CompareFunc<CharactersObject,
                      std::greater_equal<std::string>>("string>=?")));
  scope->set_value("string>=?", greater_equal_chars);

  static ObjectPtr<CallableObject> equal_chars(
      make_simple_callable(
          CompareFunc<CharactersObject,
                      std::equal_to<std::string>>("string=?")));
  scope->set_value("string=?", equal_chars);

  // Misc
  static ObjectPtr<CallableObject> print(make_simple_callable(print_function));
  scope->set_value("print", print);

  static ObjectPtr<CallableObject> exit(make_simple_callable(exit_function));
  scope->set_value("exit", exit);

  static ObjectPtr<CallableObject> throw_(make_simple_callable(throw_function));
  scope->set_value("throw", throw_);

  scope->set_value("null", nullptr);
}

void init_scope_with_stdlibs(const std::shared_ptr<Scope>& scope) {
  VirtualMachine<> vm(scope);
  vm.eval_all(builtins::kBuiltinsStdlib_common);
}

} // lispp
// end file ../src/core/builtins.cpp



// file ../src/core/callable_object.cpp
namespace lispp {

std::string CallableObject::to_string() const {
  if (type_ == CallableType::kFunction) {
    return "<function>";
  } else {
    return "<macro>";
  }
}

ObjectPtr<> CallableObject::execute(const std::shared_ptr<Scope>& scope,
                                    const ObjectPtr<>& args) {
  ObjectPtr<> prepared_args(prepare_args(args, scope));

  std::shared_ptr<Scope> local_scope = scope;
  if (create_separate_scope_) {
    local_scope = scope->create_child_scope();
  }

  auto unpacked_args = unpack_list(prepared_args);

  return execute_impl(local_scope, unpacked_args);
}

ObjectPtr<> CallableObject::prepare_args(
    const ObjectPtr<>& src_args, const std::shared_ptr<Scope>& scope) const {

  if (type_ == CallableType::kFunction) {
    ObjectPtr<> sources(src_args);
    return map_list(src_args, [&scope](ObjectPtr<> object) {
        return object->eval(scope);
    });
  } else {
    return src_args;
  }
}

std::ostream& operator<<(std::ostream& out, const CallableObject& obj) {
  return (out << obj.to_string());
}

} // lispp
// end file ../src/core/callable_object.cpp



// file ../src/core/cons_object.cpp
namespace lispp {

bool ConsObject::operator==(const Object& other) const {
  const auto* other_cons = other.as_cons();
  return (other_cons != nullptr &&
          left_value_.safe_equal(other_cons->left_value_) &&
          right_value_.safe_equal(other_cons->right_value_));
}

std::string ConsObject::to_string() const {
  if (!left_value_.valid() && !right_value_.valid()) {
    return "()";
  } else {
    std::stringstream ss;
    ss << "(";
    print_as_tail(ss);
    return ss.str();
  }
}

ObjectPtr<> ConsObject::eval(const std::shared_ptr<Scope>& scope) {
  if (!left_value_.valid()) {
    throw ExecutionError("Cannot execute empty list");
  }

  ObjectPtr<> evaled_value(left_value_->eval(scope));
  if (!evaled_value.valid()) {
    throw ExecutionError(left_value_->to_string() + " is not callable");
  }
  ObjectPtr<CallableObject> callable(evaled_value->as_callable());

  if (!callable.valid()) {
    throw ExecutionError(evaled_value->to_string() + " is not callable");
  }

  return callable->execute(scope, right_value_);
}

void ConsObject::print_as_tail(std::ostream& out) const {
  out << (left_value_.valid() ? left_value_->to_string() : "nil");

  if (!right_value_.valid()) {
    out << ")";
  } else if (right_value_->as_cons()) {
    out << " ";
    right_value_->as_cons()->print_as_tail(out);
  } else {
    out << " . " << (right_value_.valid() ? right_value_->to_string() : "nil")
        << ")";
  }
}

} // lispp
// end file ../src/core/cons_object.cpp



// file ../src/core/function_utils.cpp
namespace lispp {

bool is_true_value(const ObjectPtr<>& object) {
  ObjectPtr<BooleanObject> bool_object;
  if (object != nullptr) {
    bool_object = object->as_boolean();
  }

  return (bool_object == nullptr) || bool_object->get_value();
}

bool is_true_condition(const ObjectPtr<>& condition,
                       const std::shared_ptr<Scope>& scope) {
  ObjectPtr<> eval_result;
  if (condition != nullptr) {
    eval_result = condition->eval(scope);
  }

  return is_true_value(eval_result);
}

ObjectPtr<> safe_eval(const ObjectPtr<>& object,
                      const std::shared_ptr<Scope>& scope) {
  return (object != nullptr ? object->eval(scope) : nullptr);
}

void check_args_count(const std::string& function_name,
                      std::size_t args_count,
                      std::size_t expected_args_count,
                      CallableType type) {
  if (args_count != expected_args_count) {
    std::stringstream ss;
    ss << (type == CallableType::kFunction ? "Function " : "Macro ")
       << function_name
       << "requires exactly " << expected_args_count << " but "
       << args_count << " given";
    throw ExecutionError(ss.str());
  }
}

void check_args_count(const std::string& function_name,
                      std::size_t args_count,
                      std::size_t expected_args_count_min,
                      std::size_t expected_args_count_max,
                      CallableType type) {
  if (args_count < expected_args_count_min ||
      args_count > expected_args_count_max) {
    std::stringstream ss;
    ss << (type == CallableType::kFunction ? "Function " : "Macro ")
       << function_name
       << " requires " << expected_args_count_min;
    if (expected_args_count_max != kInfiniteArgs) {
      ss << " up to " << expected_args_count_max;
    }
    ss << " args but " << args_count << " given";
    throw ExecutionError(ss.str());
  }
}

} // lispp
// end file ../src/core/function_utils.cpp


#include <map>
#include <unordered_map>

// file ../src/core/istream_tokenizer.cpp
namespace lispp {

IstreamTokenizer::IstreamTokenizer(std::istream& input) {
  set_input_stream(input);
}

IstreamTokenizer::~IstreamTokenizer() {}

Token IstreamTokenizer::next_token() {
  check_input_stream();

  peek_token();
  current_token_ = next_token_;
  next_token_ = Token();
  return current_token_;
}

Token IstreamTokenizer::peek_token() {
  check_input_stream();

  if (next_token_.type == TokenType::kUndefined ||
      next_token_.type == TokenType::kEnd) {
    next_token_ = parse_token();
  }

  return next_token_;
}

Token IstreamTokenizer::current_token() {
  return current_token_;
}

bool IstreamTokenizer::has_more_tokens() {
  skip_whitespaces(false);
  return input_->good();
}

int IstreamTokenizer::get_current_line() {
  return current_line_;
}

void IstreamTokenizer::clear() {}

void IstreamTokenizer::set_input_stream(std::istream& input) {
  // NOTE: Allow change input on runtime? Clear current token?
  //       Too much questions...
  input_ = &input;

  next_token_ = Token();
  // current_token_ = Token();
}

void IstreamTokenizer::check_input_stream() {
  if (input_ == nullptr) {
    throw std::runtime_error("Input stream is not set!");
  }
}

Token IstreamTokenizer::parse_token() {
  skip_whitespaces(false);

  const int current_char = input_->peek();
  if (!input_->good()) {
    return Token(TokenType::kEnd);
  } else if (current_char == '\n') {
    input_->get();
    ++current_line_;
    return Token(TokenType::kEndLine);
  } else if (current_char == '"') {
    return parse_characters_token();
  } else if (is_symbol_token_start(current_char)) {
    return parse_symbol_token();
  } else if (IsDigitExt(current_char)) {
    return parse_number_token();
  } else {
    return parse_one_symbol_token();
  }
}

Token IstreamTokenizer::parse_characters_token() {
  expect_char('"');
  std::string token_value = read_while(
      IsNotQuotes, "Unexpected end of file on reading string");
  expect_char('"');

  return Token(TokenType::kCharacters, token_value);
}

bool IstreamTokenizer::is_symbol_token_start(char current_char) {
  return IsInitialOfSymbol(current_char) ||
      (IsSign(current_char) && !IsDigitExt(peek_next_char()));
}

Token IstreamTokenizer::parse_symbol_token() {
  std::string value = read_while(IsSymbolChar);

  if (IsSign(value[0]) && value.size() != 1) {
    throw TokenizerError("Invalid identifier token '" + value + "'");
  }

  return Token(TokenType::kSymbol, value);
}

Token IstreamTokenizer::parse_number_token() {
  std::string string_value = read_while(IsDigitExt);

  if (string_value.find('+', 1) != std::string::npos ||
      string_value.find('-', 1) != std::string::npos) {
    throw TokenizerError("Invalid number token '" + string_value + "'");
  }

  if (string_value == ".") {
    return Token(TokenType::kDot);
  }

  const auto dotpos = string_value.find('.');
  if (string_value.find('.', dotpos + 1) == std::string::npos) {
    return Token(TokenType::kNumber, std::stod(string_value));
  }

  throw TokenizerError("Invalid number token '" + string_value + "'");
}

Token IstreamTokenizer::parse_one_symbol_token() {
  const auto current_char = input_->get();

  static const std::unordered_map<char, TokenType> kOneSymbolTokens{
    {',',  TokenType::kComma},
    {'`',  TokenType::kBackTick},
    {'.',  TokenType::kDot},
    {'\'', TokenType::kQuote},
    {'(',  TokenType::kOpenBracket},
    {')',  TokenType::kCloseBracket}
  };

  const auto one_symbol_token_iter = kOneSymbolTokens.find(current_char);
  if (one_symbol_token_iter != kOneSymbolTokens.end()) {
    return Token(one_symbol_token_iter->second);
  }

  throw TokenizerError("Unexpected symbol: "
                       "'" + std::string(1, current_char) + "'" +
                       " (" + std::to_string(int(current_char)) + ")");
}

void IstreamTokenizer::skip_whitespaces(bool with_eol) {
  while (input_->good() && IsWhiteSpace(input_->peek(), with_eol)) {
    input_->get();
  }
}

void IstreamTokenizer::expect_char(char c, bool extract) {
  if (input_->peek() != c) {
    throw TokenizerError("Expected '" + std::string(1, c)
                         + "' got '" + std::string(1, input_->peek()) + "'");
  }

  if (extract) {
    input_->get();
  }
}

int IstreamTokenizer::peek_next_char() {
  input_->get();
  const int next_char = input_->peek();
  input_->unget();

  return next_char;
}

} // lispp
// end file ../src/core/istream_tokenizer.cpp


// file ../src/core/list_utils.cpp
namespace lispp {

std::vector<ObjectPtr<>> unpack_list(const ObjectPtr<>& lst) {
  std::vector<ObjectPtr<>> result;
  auto rest = unpack_list_rest(lst, &result);
  if (rest.valid()) {
    throw ExecutionError("Unpack list: unexpected list tail");
  }
  return result;
}

ObjectPtr<> unpack_list_rest(const ObjectPtr<>& lst,
                             std::vector<ObjectPtr<>>* result) {
  return unpack_list(lst, kInfiniteValuesNum, result);
}

ObjectPtr<> unpack_list(const ObjectPtr<>& lst, int num_values,
                        std::vector<ObjectPtr<>>* result) {
  auto tail = lst;
  auto cons_tail = tail.safe_cast<ConsObject>();

  while (num_values != 0 && cons_tail.valid()) {
    result->push_back(cons_tail->get_left_value());

    tail = cons_tail->get_right_value();
    cons_tail = tail.safe_cast<ConsObject>();

    --num_values;
  }

  return tail;
}

ObjectPtr<> pack_list(const std::vector<ObjectPtr<>>& lst,
                      const ObjectPtr<>& rest) {
  ObjectPtr<> result = rest;
  for (auto it = lst.rbegin(); it != lst.rend(); ++it) {
    ObjectPtr<ConsObject> new_head(new ConsObject(*it, result));
    result = new_head;
  }
  return result;
}

ObjectPtr<> pack_list(const std::vector<ObjectPtr<>>& lst) {
  return pack_list(lst, nullptr);
}

} // lispp
// end file ../src/core/list_utils.cpp


// file ../src/core/object.cpp
namespace lispp {

Object::~Object() { }

std::string Object::GetTypeName() {
  return "object";
}

int Object::get_ref_count() const {
  return ref_count_;
}

int Object::ref() {
  return ++ref_count_;
}

int Object::unref() {
  const int result = unref_nodelete();
  if (result == 0) {
    delete this;
  }
  return result;
}

int Object::unref_nodelete() {
  assert(ref_count_ > 0);
  return --ref_count_;
}

std::ostream& operator<<(std::ostream& out, const Object& obj) {
  return (out << obj.to_string());
}

std::ostream& operator<<(std::ostream& out, const DecoratorObject& obj) {
  return (out << obj.to_string());
}

} // lispp
// end file ../src/core/object.cpp



// file ../src/core/parser.cpp
namespace lispp {

Parser::Parser(ITokenizer* tokenizer)
    : tokenizer_(tokenizer) {}

bool Parser::has_objects(bool ignore_endlines) {
  if (ignore_endlines) {
    skip_endlines();
  }

  return (tokenizer_->peek_token().type != TokenType::kEnd) &&
         (tokenizer_->peek_token().type != TokenType::kEndLine);
}

ObjectPtr<> Parser::parse_object() {
  skip_endlines();
  const auto current_token = tokenizer_->next_token();

  if (current_token.type == TokenType::kNumber) {
    double value = current_token.number_value;
    return new NumberObject(value);

  } else if (current_token.type == TokenType::kCharacters) {
    std::string value = current_token.string_value;
    return new CharactersObject(value);

  } else if (current_token.type == TokenType::kSymbol) {
    std::string value = current_token.string_value;
    if (value == "#t") {
      return new BooleanObject(true);
    } else if (value == "#f") {
      return new BooleanObject(false);
    } else {
      return new SymbolObject(value);
    }

  } else if (current_token.type == TokenType::kQuote) {
    ObjectPtr<> underlying_object(parse_object());
    return new QuoteObject(underlying_object);

  } else if (current_token.type == TokenType::kComma) {
    ObjectPtr<> underlying_object(parse_object());
    return new CommaObject(underlying_object);

  } else if (current_token.type == TokenType::kBackTick) {
    ObjectPtr<> underlying_object(parse_object());
    return new BackTickObject(underlying_object);

  } else if (current_token.type == TokenType::kOpenBracket) {
    return parse_begun_list();

  } else if (current_token.type == TokenType::kEnd) {
    return nullptr;

  }

  std::stringstream ss;
  ss << "Unexpected token: " << current_token;
  throw ParserError(ss.str());
}

ObjectPtr<ConsObject> Parser::parse_begun_list() {
  skip_endlines();
  if (tokenizer_->peek_token().type == TokenType::kCloseBracket) {
    tokenizer_->next_token();
    return nullptr;
  }

  ObjectPtr<> left_object(parse_object());
  ObjectPtr<ConsObject> list_item(new ConsObject(left_object));
  Token current_token = tokenizer_->peek_token();

  if (current_token.type == TokenType::kDot) {
    tokenizer_->next_token();
    list_item->set_right_value(parse_object());
    expect_token(Token(TokenType::kCloseBracket), true);
  } else if (current_token.type == TokenType::kEnd) {
    throw ParserError("Unexpected end of file");
  } else {
    list_item->set_right_value(parse_begun_list());
  }

  return list_item;
}

void Parser::expect_token(const Token& tok, bool extract) {
  if (tok != tokenizer_->peek_token()) {
    std::stringstream ss;
    ss << "Expected " << tok << " got " << tokenizer_->peek_token();
    throw ParserError(ss.str());
  }

  if (extract) {
    tokenizer_->next_token();
  }
}

void Parser::skip_endlines() {
  while (tokenizer_->peek_token().type == TokenType::kEndLine) {
    tokenizer_->next_token();
  }
}

} // lispp
// end file ../src/core/parser.cpp



// file ../src/core/scope.cpp
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
// end file ../src/core/scope.cpp


// file ../src/core/string_tokenizer.cpp
namespace lispp {

StringTokenizer::StringTokenizer() {
  set_input_stream(input_ss_);
}

StringTokenizer::StringTokenizer(const std::string& initial_value) {
  set_input_stream(input_ss_);
  append(initial_value);
}

void StringTokenizer::clear() {
  input_ss_.clear();
  next_token(); // NOTE: eat cached token
  next_token();
}

void StringTokenizer::append(const std::string& value) {
  /* NOTE: we should clear the stringstream to clear flags.
   *       This shouldn't break the underlying tokenizer
   *       if stream is already ended
   */
  if (!input_ss_.good()) {
    input_ss_.clear();
  }

  input_ss_<< value;
}

} // lispp
// end file ../src/core/string_tokenizer.cpp


#include <map>

// file ../src/core/token.cpp
namespace lispp {

Token::Token(TokenType type) : type(type) {}

Token::Token(TokenType type, double number_value)
    : type(type), number_value(number_value) {}

Token::Token(TokenType type, const std::string& string_value)
    : type(type), string_value(string_value) {}


bool Token::operator==(const Token& other) const {
  return (type == other.type) && (number_value == other.number_value) &&
         (string_value == other.string_value);
}

bool Token::operator!=(const Token& other) const {
  return !(*this == other);
}

std::ostream& operator<<(std::ostream& out, const Token& token) {
  static const std::map<TokenType, std::string> kTypeNames{
    {TokenType::kNumber,       "Number"},
    {TokenType::kCharacters,   "Character"},
    {TokenType::kSymbol,       "Symbol"},
    {TokenType::kComma,        "Comma"},
    {TokenType::kBackTick,     "BackTick"},
    {TokenType::kDot,          "Dot"},
    {TokenType::kQuote,        "Quote"},
    {TokenType::kOpenBracket,  "OpenBracket"},
    {TokenType::kCloseBracket, "CloseBracket"},
    {TokenType::kEndLine,      "EndLine"},
    {TokenType::kEnd,          "End"},
    {TokenType::kUndefined,    "Undefined"}
  };

  std::string type_name = "<unknown>";
  auto type_name_iter = kTypeNames.find(token.type);
  if (type_name_iter != kTypeNames.end()) {
    type_name = type_name_iter->second;
  }

  out << "(" << type_name;
  if (token.type == TokenType::kNumber) {
    out << " " << token.number_value;
  } else if (token.type == TokenType::kCharacters) {
    out << " \"" << token.string_value << "\"";
  } else if (token.type == TokenType::kSymbol) {
    out << " " << token.string_value;
  }
  out << ")";

  return out;
}

} // lispp
// end file ../src/core/token.cpp


// file ../src/core/tokenizer.cpp
namespace lispp {

ITokenizer::~ITokenizer() {}

} // lispp
// end file ../src/core/tokenizer.cpp


#include <sstream>


// file ../src/core/user_callable_object.cpp
namespace lispp {

UserCallableObject::UserCallableObject(
    const std::string& name, const std::vector<std::string>& args,
    const std::vector<ObjectPtr<>>& body, const std::shared_ptr<Scope>& closure,
    const std::string& rest_arg_name, CallableType type)
    : CallableObject(type, true), name_(name), args_(args), body_(body),
    closure_(closure), rest_arg_name_(rest_arg_name) {}

ObjectPtr<> UserCallableObject::execute_impl(
    const std::shared_ptr<Scope>& scope,
    const std::vector<ObjectPtr<>>& args) {
  if (args.size() < args_.size() ||
      (rest_arg_name_.empty() && args.size() > args_.size())) {
    std::stringstream ss;
    ss << name_ << " "
       << (get_type() == CallableType::kFunction ? "function" : "macro")
       << " expects " << args_.size() << " arguments but " << args.size() << " given";
    throw ExecutionError(ss.str());
  }

  std::shared_ptr<Scope> local_scope = closure_->create_child_scope();
  for (std::size_t arg_index = 0; arg_index < args_.size(); ++arg_index) {
    local_scope->set_value(args_[arg_index], args[arg_index]);
  }

  if (!rest_arg_name_.empty()) {
    std::vector<ObjectPtr<>> rest_args(
        std::next(args.begin(), args_.size()), args.end());
    auto rest_lst = pack_list(rest_args);
    local_scope->set_value(rest_arg_name_, rest_lst);
  }

  ObjectPtr<> result;
  for (auto body_expression : body_) {
    result = body_expression.safe_eval(local_scope);
    // NOTE: this scope? It's strange place but macroses are evaluated twice?
    if (get_type() == CallableType::kMacro) {
      result = result.safe_eval(scope);
    }
  }

  return result;
}

} // lispp
// end file ../src/core/user_callable_object.cpp



// file ../src/core/virtual_machine_base.cpp
namespace lispp {

ObjectPtr<> VirtualMachineBase::parse() {
  try {
    return parser_->parse_object();
  } catch (const ParserError&) {
    tokenizer_->clear();
    throw;
  }
}

ObjectPtr<> VirtualMachineBase::eval() {
  auto object = parse();
  return object.safe_eval(global_scope_);
}

ObjectPtr<> VirtualMachineBase::eval_all() {
  ObjectPtr<> result;
  while (parser_->has_objects()) {
    result = eval();
  }

  return result;
}

Parser& VirtualMachineBase::get_parser() {
  return *parser_;
}
std::shared_ptr<Scope> VirtualMachineBase::get_global_scope() {
  return global_scope_;
}

VirtualMachineBase::VirtualMachineBase(
    const std::shared_ptr<Scope>& global_scope, ITokenizer* tokenizer) {
  init(tokenizer);

  global_scope_ = global_scope;
}

VirtualMachineBase::VirtualMachineBase(ITokenizer* tokenizer) {
  init(tokenizer);

  global_scope_.reset(new Scope);
  init_global_scope(global_scope_);
}

void VirtualMachineBase::init(ITokenizer* tokenizer) {
  tokenizer_ = tokenizer;
  parser_.reset(new Parser(tokenizer_));
}

ITokenizer* VirtualMachineBase::get_tokenizer_base() {
  return tokenizer_;
}

} // lispp
// end file ../src/core/virtual_machine_base.cpp

// file ../build/stdlib/common_stdlib_module.cpp
// This file was generated from *.lisp sources on compile-time

namespace lispp {
namespace builtins {

const char* kBuiltinsStdlib_common =
    "(define (empty lst) (null? lst))"
    "(define (empty? lst) (null? lst))"
    "(define (pair? lst) (cons? lst))"
    ""
    "(define (cadr lst) (cdr (car lst)))"
    "(define (caddr lst) (cdr (cdr (car lst))))"
    "(define (cadddr lst) (cdr (cdr (cdr (car lst)))))"
    ""
    "(define (first lst) (car lst))"
    "(define (rest lst) (cdr lst))"
    "(define (second lst) (cadr lst))"
    "(define (third lst) (caddr lst))"
    "(define (fourth lst) (cadddr lst))"
    ""
    "(define (list . x) x)"
    ""
    "(define (length lst)"
    "  (if (null? lst)"
    "    0"
    "    (+ (length (cdr lst)) 1)))"
    ""
    "(define (list-tail lst n)"
    "  (if (= n 0)"
    "    lst"
    "    (list-tail (cdr lst) (- n 1))))"
    ""
    "(define (list-ref lst n)"
    "  (car (list-tail lst n)))"
    ""
    "(define (take lst n)"
    "  (define (take-impl lst lst-res n)"
    "    (if (= n 0)"
    "      lst-res"
    "      (take-impl (cdr lst) (cons (car lst) lst-res) (- n 1))))"
    "  (reverse (take-impl lst '() n)))"
    ""
    "(define (append lst x)"
    "  (if (null? lst)"
    "    x"
    "    (cons (car lst) (append (cdr lst) x))))"
    ""
    "(define (reverse lst)"
    "  (define (reverse-impl lst lst-res)"
    "    (if (null? lst)"
    "      lst-res"
    "      (reverse-impl (cdr lst) (cons (car lst) lst-res))))"
    "  (reverse-impl lst '()))"
    ""
    "(define (map proc lst)"
    "  (if (null? lst)"
    "    lst"
    "    (cons (proc (car lst)) (map proc (cdr lst)))))"
    ""
    "(define (filter proc lst)"
    "  (cond"
    "    ((null? lst) lst)"
    "    ((proc (car lst)) (cons (car lst) (filter proc (cdr lst))))"
    "    (#t (filter prox (cdr lst)))))"
    ""
    "(define (member v lst)"
    "  (if (null? lst)"
    "    #f"
    "    (or (= (car lst) v) (member v (cdr lst)))))"
    ""
    "(define (foldr op lst)"
    "  (cond"
    "    ((null? lst) '())"
    "    ((null? (cdr lst)) (car lst))"
    "    (#t (op (car lst) (foldr op (cdr lst))))))"
    ""
    "(define (make-list n v)"
    "  (if (= n 0)"
    "    '()"
    "    (cons v (make-list (- n 1)))))"
    ""
    ""
    "(define (max frst . rest)"
    "  (define (max2 x y) (if (> x y) x y))"
    "  (if (not (number? frst)) (throw \"number expected, got \" frst))"
    "  (foldr max2 (cons frst rest)))"
    ""
    "(define (min frst . rest)"
    "  (define (min2 x y) (if (< x y) x y))"
    "  (if (not (number? frst)) (throw \"number expected, got \" frst))"
    "  (foldr min2 (cons frst rest)))"
    ""
    "(define (abs x)"
    "  (if (< x 0) (- x) x))";

} // builtins
} // lispp

// end file ../build/stdlib/common_stdlib_module.cpp

#include <iostream>
#include <memory>


#include <fstream>


// file ../include/lispp/file_tokenizer.h
namespace lispp {

class FileTokenizer : public IstreamTokenizer {
public:
  FileTokenizer(const std::string& filename)
      : input_file_(filename) {
    set_input_stream(input_file_);
  }

private:
  std::ifstream input_file_;
};

} // lispp
// end file ../include/lispp/file_tokenizer.h


// file ../src/repl/main.cpp
// TODO: command objects
void RunAsRepl() {
  lispp::VirtualMachine<lispp::IstreamTokenizer> vm(std::cin);

  std::cout << "> ";
  while (vm.get_parser().has_objects()) {
    try {
      auto result_object = vm.eval();
      if (result_object.valid()) {
        std::cout << *result_object << std::endl;
      }
    } catch (const lispp::TokenizerError& e) {
      std::cout << "TokenizerError: " << e.what() << std::endl;
    } catch (const lispp::ParserError& e) {
      std::cout << "ParserError: " << e.what() << std::endl;
    } catch (const lispp::ExecutionError& e) {
      std::cout << "ExecutionError: " << e.what() << std::endl;
    } catch (const lispp::ScopeError& e) {
      std::cout << "ScopeError: " << e.what() << std::endl;
    } catch (const std::exception& e) {
      std::cout << "Unknown exception: " << e.what() << std::endl;
    } catch (...) {
      std::cout << "Unknown error." << std::endl;
    }

    std::cout << "> ";
  }
}

void RunFromFile(const std::string filename) {
  lispp::VirtualMachine<lispp::FileTokenizer> vm(filename);

  try {
    vm.eval_all();
  } catch (const lispp::TokenizerError& e) {
    std::cout << "TokenizerError at line "
              << vm.get_tokenizer().get_current_line() << ": "
              << e.what() << std::endl;
  } catch (const lispp::ParserError& e) {
    std::cout << "ParserError at line "
              << vm.get_tokenizer().get_current_line() << ": "
              << e.what() << std::endl;
  } catch (const lispp::ExecutionError& e) {
    std::cout << "ExecutionError: " << e.what() << std::endl;
  } catch (const lispp::ScopeError& e) {
    std::cout << "ScopeError: " << e.what() << std::endl;
  } catch (const std::exception& e) {
    std::cout << "Unknown exception: " << e.what() << std::endl;
  } catch (...) {
    std::cout << "Unknown error." << std::endl;
  }
}

int main(int argc, const char* argv[]) {
  if (argc == 1) {
    RunAsRepl();
  } else {
    RunFromFile(argv[1]);
  }

  return 0;
}
// end file ../src/repl/main.cpp

