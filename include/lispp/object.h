#pragma once

#include <cassert>
#include <iostream>
#include <memory>
#include <string>

// #include <lispp/scope.h>

#include <lispp/object_ptr.h>

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

#include <lispp/object-impl.h>
#include <lispp/object_ptr-impl.h>
