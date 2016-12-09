#pragma once

#include <type_traits>
#include <memory>

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

#include <lispp/object.h> // NOTE: will include lispp/object_ptr-impl.h
