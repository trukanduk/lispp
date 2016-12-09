#include <lispp/list_utils.h>

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
