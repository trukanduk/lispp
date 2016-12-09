#include <lispp/function_utils.h>

#include <lispp/objects_all.h>

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
