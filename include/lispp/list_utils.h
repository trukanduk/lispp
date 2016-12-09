#pragma once

#include <vector>

#include <lispp/object.h>
#include <lispp/cons_object.h>
#include <lispp/object_ptr.h>

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
