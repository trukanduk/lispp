#pragma once

#include <lispp/object.h>

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
