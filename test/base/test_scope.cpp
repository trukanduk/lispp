#include <gtest/gtest.h>

#include <lispp/scope.h>
#include <lispp/number_object.h>

using namespace lispp;

class ScopeTest : public ::testing::Test {
public:
  void SetUp() override {
    scope.reset(new Scope);
  }

protected:
  std::shared_ptr<Scope> scope;
};

TEST_F(ScopeTest, HasParentScope) {
  EXPECT_FALSE(scope->has_parent_scope());
}

TEST_F(ScopeTest, EmptyGetter) {
  EXPECT_THROW(scope->get_value("foo"), ScopeError);
}

TEST_F(ScopeTest, Setter) {
  EXPECT_NO_THROW(scope->set_value("foo", nullptr));
  EXPECT_FALSE(scope->get_value("foo").valid());
  EXPECT_THROW(scope->get_value("bar"), ScopeError);
}

TEST_F(ScopeTest, DoubleSetter) {
  ObjectPtr<> object1(new NumberObject(1));
  ObjectPtr<> object2(new NumberObject(2));

  EXPECT_NO_THROW(scope->set_value("foo", object1));
  EXPECT_NO_THROW(scope->set_value("bar", object2));

  EXPECT_EQ(2, object1->get_ref_count());
  EXPECT_EQ(2, object2->get_ref_count());

  EXPECT_EQ(object1, scope->get_value("foo"));
  EXPECT_EQ(object2, scope->get_value("bar"));
}

TEST_F(ScopeTest, OverrideSetter) {
  ObjectPtr<> object1(new NumberObject(1));
  ObjectPtr<> object2(new NumberObject(2));

  EXPECT_NO_THROW(scope->set_value("foo", object1));
  EXPECT_NO_THROW(scope->set_value("foo", object2));

  EXPECT_EQ(1, object1->get_ref_count());
  EXPECT_EQ(2, object2->get_ref_count());

  EXPECT_EQ(object2, scope->get_value("foo"));
}

TEST_F(ScopeTest, Replace_NoValue) {
  EXPECT_THROW(scope->replace_value("foo", nullptr), ScopeError);
}

TEST_F(ScopeTest, Replace_HasValue) {
  ObjectPtr<> object1(new NumberObject(1));
  ObjectPtr<> object2(new NumberObject(2));
  EXPECT_NO_THROW(scope->set_value("foo", object1));

  EXPECT_NO_THROW(scope->replace_value("foo", object2));

  EXPECT_EQ(1, object1->get_ref_count());
  EXPECT_EQ(2, object2->get_ref_count());

  EXPECT_EQ(object2, scope->get_value("foo"));
}

class NestedScopeTest : public ::testing::Test {
public:
  void SetUp() override {
    parent_scope.reset(new Scope);
    scope.reset(new Scope(parent_scope));

    parent_scope->set_value("foo", nullptr);
  }

protected:
  std::shared_ptr<Scope> parent_scope;
  std::shared_ptr<Scope> scope;
};

TEST_F(NestedScopeTest, HasParentScope) {
  EXPECT_TRUE(scope->has_parent_scope());
  EXPECT_FALSE(parent_scope->has_parent_scope());
}

TEST_F(NestedScopeTest, HasValue) {
  EXPECT_TRUE(scope->has_value("foo"));
  EXPECT_FALSE(scope->has_value("bar"));
}

TEST_F(NestedScopeTest, GetValue) {
  EXPECT_FALSE(scope->get_value("foo").valid());
  EXPECT_FALSE(parent_scope->get_value("foo").valid());
}

TEST_F(NestedScopeTest, SetValue) {
  ObjectPtr<> object(new NumberObject(123));
  EXPECT_NO_THROW(scope->set_value("bar", object));

  EXPECT_EQ(2, object->get_ref_count());

  EXPECT_EQ(object, scope->get_value("bar"));
  EXPECT_THROW(parent_scope->get_value("bar"), ScopeError);
}

TEST_F(NestedScopeTest, ReplaceValue_ParentScope) {
  ObjectPtr<> object1(new NumberObject(1));
  ObjectPtr<> object2(new NumberObject(2));

  EXPECT_NO_THROW(parent_scope->set_value("foo", object1));
  EXPECT_NO_THROW(scope->replace_value("foo", object2));

  EXPECT_EQ(1, object1->get_ref_count());
  EXPECT_EQ(2, object2->get_ref_count());

  EXPECT_EQ(object2, parent_scope->get_value("foo"));
  EXPECT_EQ(object2, scope->get_value("foo"));
}

TEST_F(NestedScopeTest, ReplaceValue_Current) {
  ObjectPtr<> object1(new NumberObject(1));
  ObjectPtr<> object2(new NumberObject(2));
  ObjectPtr<> object3(new NumberObject(3));

  EXPECT_NO_THROW(parent_scope->set_value("foo", object1));
  EXPECT_NO_THROW(scope->set_value("foo", object2));

  EXPECT_NO_THROW(scope->replace_value("foo", object3));

  EXPECT_EQ(2, object1->get_ref_count());
  EXPECT_EQ(1, object2->get_ref_count());
  EXPECT_EQ(2, object3->get_ref_count());

  EXPECT_EQ(object1, parent_scope->get_value("foo"));
  EXPECT_EQ(object3, scope->get_value("foo"));
}
