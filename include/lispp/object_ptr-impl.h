#pragma once

#include <lispp/object_ptr.h>
#include <lispp/object.h>

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
