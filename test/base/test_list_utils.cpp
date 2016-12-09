#include <sstream>
#include <gtest/gtest.h>

#include <lispp/object_ptr.h>
#include <lispp/object.h>
#include <lispp/cons_object.h>
#include <lispp/list_utils.h>
#include <lispp/number_object.h>

using namespace lispp;

TEST(TestListUtils, MakeList_Empty) {
  EXPECT_FALSE(make_list().valid());
  EXPECT_FALSE(make_list(nullptr).valid());
}

TEST(TestListUtils, MakeList_Atom) {
  ObjectPtr<> tail(new NumberObject(1));
  EXPECT_EQ(tail, make_list(tail));
}

TEST(TestListUtils, MakeList_OneElementList) {
  ObjectPtr<> tail(new ConsObject(new NumberObject(1)));
  EXPECT_EQ(tail, make_list(tail));
}

TEST(TestListUtils, MakeList_Cons) {
  ObjectPtr<> object1(new NumberObject(1));
  ObjectPtr<> object2(new NumberObject(2));

  ObjectPtr<> ref_value(new ConsObject(new NumberObject(1),
                                       new NumberObject(2)));

  ObjectPtr<> result(make_list(object1, object2));
  EXPECT_TRUE(result.valid());
  EXPECT_EQ(*ref_value, *result);
}

TEST(TestListUtils, MakeList_TwoEleentList) {
  ObjectPtr<> object1(new NumberObject(1));
  ObjectPtr<> object2(new NumberObject(2));

  ObjectPtr<> ref_value(
      new ConsObject(
          new NumberObject(1),
          new ConsObject(
              new NumberObject(2))));

  ObjectPtr<> result(make_list(object1, object2, nullptr));
  EXPECT_TRUE(result.valid());
  EXPECT_EQ(*ref_value, *result);
}

TEST(TestListUtils, MapList_EmptyList) {
  ObjectPtr<> result(map_list(
      ObjectPtr<>(),
      [](const ObjectPtr<>& object) -> ObjectPtr<> {
        EXPECT_FALSE(true) << "Mustn't be performed! object is " << object;
        return nullptr;
      }
  ));
  EXPECT_FALSE(result.valid());
}

TEST(TestListUtils, MapList_Atom) {
  ObjectPtr<> lst(make_list(new NumberObject(123)));
  ObjectPtr<> ref_object(new NumberObject(123));

  ObjectPtr<> result(map_list(
      lst,
      [&ref_object](const ObjectPtr<>& object) -> ObjectPtr<> {
        EXPECT_TRUE(object.valid());
        if (object.valid()) {
          EXPECT_EQ(*ref_object, *object);
        }
        return object;
      }
  ));
  EXPECT_TRUE(result.valid());
  EXPECT_EQ(*lst, *result);
}

TEST(TestListUtils, MapList_SingleCons) {
  std::vector<ObjectPtr<>> objects{
      ObjectPtr<>(new NumberObject(123)),
      ObjectPtr<>(new NumberObject(321))
  };
  ObjectPtr<> source_lst(make_list(objects[0], objects[1]));
  ObjectPtr<> ref_lst(make_list(new NumberObject(124),
                                new NumberObject(322)));

  int index = 0;
  auto func = [&index, &objects](const ObjectPtr<>& object) mutable
      -> ObjectPtr<> {
    EXPECT_LE(index, static_cast<int>(objects.size()));
    EXPECT_EQ(objects[index], object);
    ++index;
    return new NumberObject(object->as_number()->get_value() + 1);
  };

  ObjectPtr<> result(map_list(source_lst, func));
  EXPECT_TRUE(result.valid());
  EXPECT_EQ(*ref_lst, *result);
}

TEST(TestListUtils, MapList_List) {
  std::vector<ObjectPtr<>> objects{
      ObjectPtr<>(new NumberObject(123)),
      ObjectPtr<>(new NumberObject(321))
  };
  ObjectPtr<> source_lst(make_list(objects[0],
                                   objects[1],
                                   nullptr));
  ObjectPtr<> ref_lst(make_list(new NumberObject(124),
                                new NumberObject(322),
                                nullptr));

  int index = 0;
  auto func = [&index, &objects](const ObjectPtr<>& object) mutable
      -> ObjectPtr<> {
    EXPECT_LE(index, static_cast<int>(objects.size()));
    EXPECT_EQ(objects[index], object);
    ++index;
    return new NumberObject(object->as_number()->get_value() + 1);
  };

  ObjectPtr<> result(map_list(source_lst, func));
  EXPECT_TRUE(result.valid());
  EXPECT_EQ(*ref_lst, *result);
}

TEST(TestListUtils, UnpackList_Empty) {
  ObjectPtr<> empty_list;

  auto result = unpack_list(empty_list);
  EXPECT_EQ(0U, result.size());
}

TEST(TestListUtils, UnpackList_OneElement) {
  ObjectPtr<> lst(new ConsObject(new NumberObject(1)));

  auto result = unpack_list(lst);
  ASSERT_EQ(1U, result.size());
  EXPECT_TRUE(result[0].safe_equal(new NumberObject(1)));
}

TEST(TestListUtils, UnpackList_MultipleElements) {
  ObjectPtr<> lst = make_list(new NumberObject(1),
                              new NumberObject(2),
                              new NumberObject(3),
                              nullptr);

  auto result = unpack_list(lst);
  ASSERT_EQ(3U, result.size());
  EXPECT_TRUE(result[0].safe_equal(new NumberObject(1)));
  EXPECT_TRUE(result[1].safe_equal(new NumberObject(2)));
  EXPECT_TRUE(result[2].safe_equal(new NumberObject(3)));
}

TEST(TestListUtils, UnpackList_Rest) {
  ObjectPtr<> lst(new ConsObject(new NumberObject(1), new NumberObject(2)));

  EXPECT_THROW(unpack_list(lst), ExecutionError);
}

TEST(TestListUtils, UnpackListRest_Empty) {
  ObjectPtr<> empty_list;

  std::vector<ObjectPtr<>> unpacked;
  auto rest = unpack_list_rest(empty_list, &unpacked);
  EXPECT_EQ(0U, unpacked.size());
  EXPECT_FALSE(rest.valid());
}

TEST(TestListUtils, UnpackListRest_SingleCons) {
  ObjectPtr<> lst(new ConsObject(new NumberObject(1), new NumberObject(2)));

  std::vector<ObjectPtr<>> unpacked;
  auto rest = unpack_list_rest(lst, &unpacked);
  ASSERT_EQ(1U, unpacked.size());
  EXPECT_TRUE(unpacked[0].safe_equal(new NumberObject(1)));

  EXPECT_TRUE(rest.safe_equal(new NumberObject(2)));
}

TEST(TestListUtils, UnpackListRest_NormalList) {
  ObjectPtr<> lst = make_list(new NumberObject(1),
                              new NumberObject(2),
                              nullptr);

  std::vector<ObjectPtr<>> unpacked;
  auto rest = unpack_list_rest(lst, &unpacked);
  ASSERT_EQ(2U, unpacked.size());
  EXPECT_TRUE(unpacked[0].safe_equal(new NumberObject(1)));
  EXPECT_TRUE(unpacked[1].safe_equal(new NumberObject(2)));

  EXPECT_FALSE(rest.valid());
}

TEST(TestListUtils, UnpackListRest_MultipleConses) {
  ObjectPtr<> lst = make_list(new NumberObject(1),
                              new NumberObject(2),
                              new NumberObject(3));

  std::vector<ObjectPtr<>> unpacked;
  auto rest = unpack_list_rest(lst, &unpacked);
  ASSERT_EQ(2U, unpacked.size());
  EXPECT_TRUE(unpacked[0].safe_equal(new NumberObject(1)));
  EXPECT_TRUE(unpacked[1].safe_equal(new NumberObject(2)));

  EXPECT_TRUE(rest.safe_equal(new NumberObject(3)));
}
