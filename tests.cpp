#include "gtest/gtest.h"
#include <stdexcept>

static int sum(int a, int b) {
  // In normal tasks, never change files with tests but here you can :)
  return a - b; // TODO: fix me
}

TEST(something, simple) {
  ASSERT_EQ(2, sum(1, 1));
}

static void throwing_func() {
  throw std::logic_error("some exception");
}

TEST(something, test_abi) {
  EXPECT_THROW(throwing_func(), std::logic_error);
}

TEST(optional_testing, default_ctor_2) {
  test_object::no_new_instances_guard g;
  optional<test_object> a;
  EXPECT_FALSE(static_cast<bool>(a));
  g.expect_no_instances();
}

TEST(optional_testing, deref_access) {
  optional<test_object> a(42);
  EXPECT_EQ(42, a->operator int());
  EXPECT_EQ(42, std::as_const(a)->operator int());
}

TEST(optional_testing, value_ctor) {
  optional<int> a(42);
  EXPECT_TRUE(static_cast<bool>(a));
  EXPECT_EQ(42, *a);
  EXPECT_EQ(42, *std::as_const(a));
}

TEST(optional_testing, reset) {
  test_object::no_new_instances_guard g;
  optional<test_object> a(42);
  EXPECT_TRUE(static_cast<bool>(a));
  a.reset();
  EXPECT_FALSE(static_cast<bool>(a));
  g.expect_no_instances();
}

TEST(optional_testing, dtor) {
  test_object::no_new_instances_guard g;
  optional<test_object> a(42);
  EXPECT_TRUE(static_cast<bool>(a));
  EXPECT_EQ(42, *a);
}

TEST(optional_testing, copy_ctor) {
  test_object::no_new_instances_guard g;
  optional<test_object> a(42);
  optional<test_object> b = a;
  EXPECT_TRUE(static_cast<bool>(b));
  EXPECT_EQ(42, *b);
}

TEST(optional_testing, copy_ctor_empty) {
  test_object::no_new_instances_guard g;
  optional<test_object> a;
  optional<test_object> b = a;
  EXPECT_FALSE(static_cast<bool>(b));
}

TEST(optional_testing, move_ctor) {
  test_object::no_new_instances_guard g;
  optional<only_moveable> a(42);
  optional<only_moveable> b = std::move(a);
  EXPECT_TRUE(static_cast<bool>(b));
  EXPECT_EQ(42, *b);
}

TEST(optional_testing, move_ctor_empty) {
  test_object::no_new_instances_guard g;
  optional<test_object> a;
  optional<test_object> b = std::move(a);
  EXPECT_FALSE(static_cast<bool>(b));
}

TEST(optional_testing, assignment_empty_empty) {
  test_object::no_new_instances_guard g;
  optional<test_object> a, b;
  b = a;
  EXPECT_FALSE(static_cast<bool>(b));
}

TEST(optional_testing, assignment_to_empty) {
  test_object::no_new_instances_guard g;
  optional<test_object> a(42), b;
  b = a;
  EXPECT_TRUE(static_cast<bool>(b));
  EXPECT_EQ(42, *b);
}

TEST(optional_testing, assignment_from_empty) {
  test_object::no_new_instances_guard g;
  optional<test_object> a, b(42);
  b = a;
  EXPECT_FALSE(static_cast<bool>(b));
}

TEST(optional_testing, assignment) {
  test_object::no_new_instances_guard g;
  optional<test_object> a(42), b(41);
  b = a;
  EXPECT_TRUE(static_cast<bool>(b));
  EXPECT_EQ(42, *b);
}

TEST(optional_testing, move_assignment_empty_empty) {
  test_object::no_new_instances_guard g;
  optional<only_moveable> a, b;
  b = std::move(a);
  EXPECT_FALSE(static_cast<bool>(b));
}

TEST(optional_testing, move_assignment_to_empty) {
  test_object::no_new_instances_guard g;
  optional<only_moveable> a(42), b;
  b = std::move(a);
  EXPECT_TRUE(static_cast<bool>(b));
  EXPECT_EQ(42, *b);
}

TEST(optional_testing, move_assignment_from_empty) {
  test_object::no_new_instances_guard g;
  optional<only_moveable> a, b(42);
  b = std::move(a);
  EXPECT_FALSE(static_cast<bool>(b));
}

TEST(optional_testing, move_assignment) {
  test_object::no_new_instances_guard g;
  optional<only_moveable> a(42), b(41);
  b = std::move(a);
  EXPECT_TRUE(static_cast<bool>(b));
  EXPECT_EQ(42, *b);
}

TEST(optional_testing, nullopt_ctor) {
  optional<test_object> a = nullopt;
  EXPECT_FALSE(static_cast<bool>(a));
}

TEST(optional_testing, nullopt_assignment) {
  optional<test_object> a(42);
  a = nullopt;
  EXPECT_FALSE(static_cast<bool>(a));
  EXPECT_TRUE(noexcept(a = nullopt));
}

struct mytype {
  mytype(int, int, int, std::unique_ptr<int>) {}
};

TEST(optional_testing, in_place_ctor) {
  optional<mytype> a(in_place, 1, 2, 3, std::unique_ptr<int>());
  EXPECT_TRUE(static_cast<bool>(a));
}

TEST(optional_testing, emplace) {
  optional<mytype> a;
  a.emplace(1, 2, 3, std::unique_ptr<int>());
  EXPECT_TRUE(static_cast<bool>(a));
}
namespace {
struct throw_in_ctor {
  struct exception : std::exception {
    using std::exception::exception;
  };

  throw_in_ctor(int, int) {
    if (enable_throw)
      throw exception();
  }

  static inline bool enable_throw = false;
};
} // namespace

TEST(optional_testing, emplace_throw) {
  optional<throw_in_ctor> a(in_place, 1, 2);
  throw_in_ctor::enable_throw = true;
  EXPECT_THROW(a.emplace(3, 4), throw_in_ctor::exception);
  EXPECT_FALSE(static_cast<bool>(a));
}

TEST(optional_testing, comparison_non_empty_and_non_empty) {
  optional<int> a(41), b(42);
  EXPECT_FALSE(a == b);
  EXPECT_TRUE(a != b);
  EXPECT_TRUE(a < b);
  EXPECT_TRUE(a <= b);
  EXPECT_FALSE(a > b);
  EXPECT_FALSE(a >= b);

  EXPECT_TRUE(a == a);
  EXPECT_FALSE(a != a);
  EXPECT_FALSE(a < a);
  EXPECT_TRUE(a <= a);
  EXPECT_FALSE(a > a);
  EXPECT_TRUE(a >= a);

  EXPECT_FALSE(b == a);
  EXPECT_TRUE(b != a);
  EXPECT_FALSE(b < a);
  EXPECT_FALSE(b <= a);
  EXPECT_TRUE(b > a);
  EXPECT_TRUE(b >= a);
}

TEST(optional_testing, comparison_non_empty_and_empty) {
  optional<int> a(41), b;
  EXPECT_FALSE(a == b);
  EXPECT_TRUE(a != b);
  EXPECT_FALSE(a < b);
  EXPECT_FALSE(a <= b);
  EXPECT_TRUE(a > b);
  EXPECT_TRUE(a >= b);

  EXPECT_FALSE(b == a);
  EXPECT_TRUE(b != a);
  EXPECT_TRUE(b < a);
  EXPECT_TRUE(b <= a);
  EXPECT_FALSE(b > a);
  EXPECT_FALSE(b >= a);
}

TEST(optional_testing, comparison_empty_and_empty) {
  optional<int> a, b;
  EXPECT_TRUE(a == b);
  EXPECT_FALSE(a != b);
  EXPECT_FALSE(a < b);
  EXPECT_TRUE(a <= b);
  EXPECT_FALSE(a > b);
  EXPECT_TRUE(a >= b);

  EXPECT_TRUE(a == a);
  EXPECT_FALSE(a != a);
  EXPECT_FALSE(a < a);
  EXPECT_TRUE(a <= a);
  EXPECT_FALSE(a > a);
  EXPECT_TRUE(a >= a);

  EXPECT_TRUE(b == a);
  EXPECT_FALSE(b != a);
  EXPECT_FALSE(b < a);
  EXPECT_TRUE(b <= a);
  EXPECT_FALSE(b > a);
  EXPECT_TRUE(b >= a);
}
namespace {
struct cvalue {
  constexpr cvalue() : value(0) {}

  constexpr cvalue(int value) : value(value) {}

  constexpr cvalue(cvalue const& other) : value(other.value) {}

  constexpr cvalue& operator=(cvalue const& other) {
    value = other.value + 1;
    return *this;
  }

  constexpr int get() const {
    return value;
  }

private:
  int value;
};
} // namespace

static_assert(std::is_trivially_destructible_v<optional<int>>);
static_assert(std::is_trivially_copyable_v<optional<int>>);

static_assert(!std::is_trivially_destructible_v<optional<std::vector<int>>>);
static_assert(!std::is_trivially_copyable_v<optional<std::vector<int>>>);

static_assert(std::is_copy_constructible_v<optional<std::vector<int>>>);
static_assert(std::is_copy_assignable_v<optional<std::vector<int>>>);
static_assert(std::is_move_constructible_v<optional<std::vector<int>>>);
static_assert(std::is_move_assignable_v<optional<std::vector<int>>>);

namespace {
struct non_copy_assignable {
  non_copy_assignable(const non_copy_assignable&) = default;
  non_copy_assignable(non_copy_assignable&&) = default;
  non_copy_assignable& operator=(const non_copy_assignable&) = delete;
  non_copy_assignable& operator=(non_copy_assignable&&) = default;
};
static_assert(std::is_copy_constructible_v<optional<non_copy_assignable>>);
static_assert(!std::is_copy_assignable_v<optional<non_copy_assignable>>);
static_assert(std::is_move_constructible_v<optional<non_copy_assignable>>);
static_assert(std::is_move_assignable_v<optional<non_copy_assignable>>);

struct non_copyable {
  non_copyable(const non_copyable&) = delete;
  non_copyable(non_copyable&&) = delete;
  non_copyable& operator=(const non_copyable&) = default;
  non_copyable& operator=(non_copyable&&) = default;
};
static_assert(!std::is_copy_constructible_v<optional<non_copyable>>);
static_assert(!std::is_move_constructible_v<optional<non_copyable>>);

struct non_assignable {
  non_assignable(const non_assignable&) = default;
  non_assignable(non_assignable&&) = default;
  non_assignable& operator=(const non_assignable&) = delete;
  non_assignable& operator=(non_assignable&&) = delete;
};
static_assert(std::is_copy_constructible_v<optional<non_assignable>>);
static_assert(!std::is_copy_assignable_v<optional<non_assignable>>);
static_assert(std::is_move_constructible_v<optional<non_assignable>>);
static_assert(!std::is_move_assignable_v<optional<non_assignable>>);

// only movable
static_assert(!std::is_copy_constructible_v<optional<std::unique_ptr<int>>>);
static_assert(!std::is_copy_assignable_v<optional<std::unique_ptr<int>>>);
static_assert(std::is_move_constructible_v<optional<std::unique_ptr<int>>>);
static_assert(std::is_move_assignable_v<optional<std::unique_ptr<int>>>);

struct only_move_constructible {
  only_move_constructible(const only_move_constructible&) = delete;
  only_move_constructible(only_move_constructible&&) = default;
  only_move_constructible& operator=(const only_move_constructible&) = delete;
  only_move_constructible& operator=(only_move_constructible&&) = delete;
};
static_assert(!std::is_copy_constructible_v<optional<only_move_constructible>>);
static_assert(!std::is_copy_assignable_v<optional<only_move_constructible>>);
static_assert(std::is_move_constructible_v<optional<only_move_constructible>>);
static_assert(!std::is_move_assignable_v<optional<only_move_constructible>>);

struct nothing {
  nothing(const nothing&) = delete;
  nothing(nothing&&) = delete;
  nothing& operator=(const nothing&) = delete;
  nothing& operator=(nothing&&) = delete;
};
static_assert(!std::is_copy_constructible_v<optional<nothing>>);
static_assert(!std::is_copy_assignable_v<optional<nothing>>);
static_assert(!std::is_move_constructible_v<optional<nothing>>);
static_assert(!std::is_move_assignable_v<optional<nothing>>);
} // namespace

static_assert([] {
  optional<cvalue> a;
  return !static_cast<bool>(a);
}());

static_assert([] {
  optional<cvalue> a(nullopt);
  return !static_cast<bool>(a);
}());

static_assert([] {
  optional<cvalue> a(42);
  return (*a).get() == 42;
}());

static_assert([] {
  optional<cvalue> a(in_place, 42);
  return (*a).get() == 42;
}());

static_assert([] {
  optional<cvalue> a(42);
  return (*std::as_const(a)).get() == 42;
}());

static_assert([] {
  optional<cvalue> a(42);
  return a->get() == 42;
}());

static_assert([] {
  optional<cvalue> a(42);
  return std::as_const(a)->get() == 42;
}());

static_assert([] {
  optional<int> a(42);
  return a == a;
}());

static_assert([] {
  optional<int> a(42), b(43);
  return a != b;
}());

static_assert([] {
  optional<int> a(42), b(43);
  return a < b;
}());

static_assert([] {
  optional<int> a(42), b(43);
  return a <= b;
}());

static_assert([] {
  optional<int> a(43), b(42);
  return a > b;
}());

static_assert([] {
  optional<int> a(43), b(42);
  return a >= b;
}());

static_assert([] {
  optional<int> a(43);
  optional<int> b(a);
  return a == b;
}());

static_assert([] {
  optional<int> a(43);
  optional<int> b(std::move(a));
  return b && *b == 43;
}());

static_assert([] {
  optional<int> a(43), b(42);
  a = b;
  return a == b;
}());

static_assert([] {
  optional<int> a(43), b(42);
  a = std::move(b);
  return *a == 42;
}());
