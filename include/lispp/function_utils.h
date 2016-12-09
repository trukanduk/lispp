#pragma once

#include <sstream>

#include <lispp/object.h>
#include <lispp/object_ptr.h>
#include <lispp/callable_object.h>
#include <lispp/scope.h>

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
