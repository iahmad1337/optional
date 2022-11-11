#pragma once

#include "member_switches.h"
#include "optional_bases.h"

/*******************************************************************************
 *                                  Optional                                   *
 *******************************************************************************/

template <typename T>
class optional
    : public detail::move_assign_base<T>,
      private detail::enable_copy_construction<std::is_copy_constructible_v<T>>,
      private detail::enable_copy_assignment<std::is_copy_assignable_v<T> &&
                                             std::is_copy_constructible_v<T>>,
      private detail::enable_move_construction<std::is_move_constructible_v<T>>,
      private detail::enable_move_assignment<std::is_move_assignable_v<T> &&
                                             std::is_move_constructible_v<T>> {
  using base = detail::move_assign_base<T>;

public:
  using base::base;
  using base::operator=;
  constexpr optional() noexcept = default;

  constexpr optional(const T& value_) : base{in_place, value_} {}

  constexpr optional(T&& value_) : base{in_place, std::move(value_)} {}

  template <typename... Args>
  constexpr optional(in_place_t, Args&&... args)
      : base{in_place, std::forward<Args>(args)...} {}

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
