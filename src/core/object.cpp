#include <lispp/object.h>

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
