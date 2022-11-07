#pragma once

#include "member_switches.h"

#include <cassert>
#include <type_traits>
#include <utility>

struct nullopt_t {};
inline constexpr nullopt_t nullopt;
struct in_place_t {};
inline constexpr in_place_t in_place;

/*******************************************************************************
 *                       Storage & destructor triviality                       *
 *******************************************************************************/

template <typename T, bool trivial = std::is_trivially_destructible_v<T>>
struct optional_storage_base {
  bool active{false};
  union {
    char dummy;
    T value;
  };

  void reset() noexcept {
    if (active) {
      value.~T();
      active = false;
    }
  }

  constexpr optional_storage_base() noexcept : active{false}, dummy{} {}

  constexpr optional_storage_base(const T& value_)
      : active{true}, value{value_} {}

  constexpr optional_storage_base(T&& value_)
      : active{true}, value{std::move(value_)} {}

  template <typename... Args>
  constexpr optional_storage_base(in_place_t, Args&&... args)
      : active{true}, value{std::forward<Args>(args)...} {}

  ~optional_storage_base() {
    reset();
  }
};

template <typename T>
struct optional_storage_base<T, true> {
  bool active{false};
  union {
    char dummy;
    T value;
  };

  void reset() noexcept {
    active = false;
  }

  constexpr optional_storage_base() noexcept : active{false}, dummy{} {}

  constexpr optional_storage_base(const T& value_)
      : active{true}, value{value_} {}

  constexpr optional_storage_base(T&& value_)
      : active{true}, value{std::move(value_)} {}

  template <typename... Args>
  constexpr optional_storage_base(in_place_t, Args&&... args)
      : active{true}, value{std::forward<Args>(args)...} {}

  // No user defined destructor => trivial
};

/*******************************************************************************
 *                          Copy construct triviality                          *
 *******************************************************************************/

template <typename T, bool trivial = std::is_trivially_copyable_v<T>>
struct copy_construct_optional_base : optional_storage_base<T> {
  using base = optional_storage_base<T>;
  using base::base;

  constexpr copy_construct_optional_base(
      const copy_construct_optional_base& other)
      : base{} {
    this->active = other.active;
    if (other.active) {
      new (&(this->value)) T{other.value};
    }
  }

  constexpr copy_construct_optional_base&
  operator=(copy_construct_optional_base const& other) = default;

  constexpr copy_construct_optional_base(copy_construct_optional_base&& other)
      : base{} {
    this->active = other.active;
    if (other.active) {
      new (&(this->value)) T{std::move(other.value)};
      other.reset();
    }
  }

  constexpr copy_construct_optional_base&
  operator=(copy_construct_optional_base&& other) {
    if (this == &other) {
      return *this;
    }
    this->reset();
    this->active = other.active;
    if (other.active) {
      new (&(this->value)) T{std::move(other.value)};
      other.reset();
    }
    return *this;
  }
};

template <typename T>
struct copy_construct_optional_base<T, true> : optional_storage_base<T> {
  using base = optional_storage_base<T>;
  using base::base;

  constexpr copy_construct_optional_base(const copy_construct_optional_base&) =
      default;
  constexpr copy_construct_optional_base&
  operator=(copy_construct_optional_base const&) = default;
  constexpr copy_construct_optional_base(copy_construct_optional_base&& other) =
      default;
  constexpr copy_construct_optional_base&
  operator=(copy_construct_optional_base&& other) = default;
};

/*******************************************************************************
 *                         Copy assignment triviality                          *
 *******************************************************************************/

template <typename T, bool trivial = std::is_trivially_copy_assignable_v<T>&&
                          std::is_trivially_copy_constructible_v<T>>
struct copy_assign_optional_base : copy_construct_optional_base<T> {
  using base = copy_construct_optional_base<T>;
  using base::base;

  constexpr copy_assign_optional_base(const copy_assign_optional_base&) =
      default;
  constexpr copy_assign_optional_base&
  operator=(copy_assign_optional_base const& other) {
    if (this == &other) {
      return *this;
    }
    this->reset();
    this->active = other.active;
    if (other.active) {
      new (&(this->value)) T{other.value};
    }
    return *this;
  }

  constexpr copy_assign_optional_base(copy_assign_optional_base&& other) =
      default;
  constexpr copy_assign_optional_base&
  operator=(copy_assign_optional_base&& other) = default;
};

template <typename T>
struct copy_assign_optional_base<T, true> : copy_construct_optional_base<T> {
  using base = copy_construct_optional_base<T>;
  using base::base;

  constexpr copy_assign_optional_base(const copy_assign_optional_base&) =
      default;
  constexpr copy_assign_optional_base&
  operator=(copy_assign_optional_base const&) = default;
  constexpr copy_assign_optional_base(copy_assign_optional_base&& other) =
      default;
  constexpr copy_assign_optional_base&
  operator=(copy_assign_optional_base&& other) = default;
};

/*******************************************************************************
 *                                  Optional                                   *
 *******************************************************************************/

template <typename T>
class optional
    : private copy_assign_optional_base<T>,
      public enable_copy_construction<std::is_copy_constructible_v<T>>,
      public enable_copy_assignment<std::is_copy_assignable_v<T> &&
                                    std::is_copy_constructible_v<T>>,
      public enable_move_construction<std::is_move_constructible_v<T>>,
      public enable_move_assignment<std::is_move_assignable_v<T> &&
                                    std::is_move_constructible_v<T>> {
  using base = copy_assign_optional_base<T>;
  using base::base;

public:
  constexpr optional() noexcept : base{} {}
  constexpr optional(nullopt_t) noexcept : base{} {}

  constexpr optional(optional const&) = default;
  constexpr optional(optional&&) = default;

  optional& operator=(optional const&) = default;
  optional& operator=(optional&&) = default;

  constexpr optional& operator=(nullopt_t) noexcept {
    this->reset();
    return *this;
  }

  constexpr explicit operator bool() const noexcept {
    return this->active;
  }

  constexpr T& operator*() noexcept {
    return this->value;
  }

  constexpr T const& operator*() const noexcept {
    return this->value;
  }

  constexpr T* operator->() noexcept {
    return &this->value;
  }

  constexpr T const* operator->() const noexcept {
    return &this->value;
  }

  template <typename... Args>
  void emplace(Args&&... args) {
    this->reset();
    new (&(this->value)) T{std::forward<Args>(args)...};
    this->active = true;
  }

  void reset() noexcept {
    this->base::reset();
  }

  [[nodiscard]] bool has_value() const noexcept {
    return this->active;
  }

  void swap(optional& other) noexcept(std::is_nothrow_move_constructible_v<T>&&
                                          std::is_nothrow_swappable_v<T>) {
    assert(this != &other);
    if (has_value() && other.has_value()) {
      using std::swap;
      swap(**this, *other);
    } else if (has_value() && !other.has_value()) {
      other = std::move(*this);
      this->reset();
    } else if (!has_value() && other.has_value()) {
      *this = std::move(other);
      other.reset();
    }
  }
};

template <typename T>
constexpr bool operator==(optional<T> const& a, optional<T> const& b) {
  if (static_cast<bool>(a) != static_cast<bool>(b)) {
    return false;
  } else if (!a && !b) {
    return true;
  } else {
    assert(a && b);
    return *a == *b;
  }
}

template <typename T>
constexpr bool operator!=(optional<T> const& a, optional<T> const& b) {
  return !(a == b);
}

template <typename T>
constexpr bool operator<(optional<T> const& a, optional<T> const& b) {
  if (!b) {
    return false;
  } else if (!a && b) {
    return true;
  } else {
    assert(a && b);
    return *a < *b;
  }
}

template <typename T>
constexpr bool operator<=(optional<T> const& a, optional<T> const& b) {
  return !(a > b);
}

template <typename T>
constexpr bool operator>(optional<T> const& a, optional<T> const& b) {
  return b < a;
}

template <typename T>
constexpr bool operator>=(optional<T> const& a, optional<T> const& b) {
  return !(a < b);
}
