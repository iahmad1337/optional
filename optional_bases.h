#pragma once

#include <cassert>
#include <type_traits>
#include <utility>

struct nullopt_t {};
inline constexpr nullopt_t nullopt;
struct in_place_t {};
inline constexpr in_place_t in_place;

namespace detail {
/*******************************************************************************
 *                       Storage & destructor triviality                       *
 *******************************************************************************/

template <typename T, bool trivial = std::is_trivially_destructible_v<T>>
struct storage_base {
  bool active{false};
  union {
    char dummy{};
    T value;
  };

  void reset() noexcept {
    if (active) {
      value.~T();
      active = false;
    }
  }

  constexpr storage_base() noexcept : active{false}, dummy{} {}
  constexpr storage_base(const storage_base&) = default;
  constexpr storage_base(storage_base&&) = default;
  constexpr storage_base& operator=(storage_base&&) = default;
  constexpr storage_base& operator=(const storage_base&) = default;

  template <typename... Args>
  constexpr storage_base(in_place_t, Args&&... args)
      : active{true}, value{std::forward<Args>(args)...} {}

  ~storage_base() {
    reset();
  }
};

template <typename T>
struct storage_base<T, true> {
  bool active{false};
  union {
    char dummy{};
    T value;
  };

  void reset() noexcept {
    active = false;
  }

  constexpr storage_base() noexcept : active{false}, dummy{} {}
  constexpr storage_base(const storage_base&) = default;
  constexpr storage_base(storage_base&&) = default;
  constexpr storage_base& operator=(storage_base&&) = default;
  constexpr storage_base& operator=(const storage_base&) = default;

  template <typename... Args>
  constexpr storage_base(in_place_t, Args&&... args)
      : active{true}, value{std::forward<Args>(args)...} {}

  // No user defined destructor => trivial
};

/*******************************************************************************
 *                          Copy construct triviality                          *
 *******************************************************************************/

template <typename T, bool trivial = std::is_trivially_copy_constructible_v<T>>
struct copy_ctor_base : storage_base<T> {
  using base = storage_base<T>;
  using base::base;

  constexpr copy_ctor_base& operator=(copy_ctor_base const&) = default;
  constexpr copy_ctor_base(copy_ctor_base&& other) = default;
  constexpr copy_ctor_base& operator=(copy_ctor_base&& other) = default;

  constexpr copy_ctor_base(const copy_ctor_base& other) : base{} {
    this->active = other.active;
    if (other.active) {
      new (&(this->value)) T{other.value};
    }
  }
};

template <typename T>
struct copy_ctor_base<T, true> : storage_base<T> {
  using base = storage_base<T>;
  using base::base;

  constexpr copy_ctor_base(const copy_ctor_base&) = default;
  constexpr copy_ctor_base& operator=(copy_ctor_base const&) = default;
  constexpr copy_ctor_base(copy_ctor_base&& other) = default;
  constexpr copy_ctor_base& operator=(copy_ctor_base&& other) = default;
};

/*******************************************************************************
 *                         Copy assignment triviality                          *
 *******************************************************************************/

template <typename T, bool trivial = std::is_trivially_copy_assignable_v<T>&&
                          std::is_trivially_copy_constructible_v<T>>
struct copy_assign_base : copy_ctor_base<T> {
  using base = copy_ctor_base<T>;
  using base::base;

  constexpr copy_assign_base(const copy_assign_base&) = default;
  constexpr copy_assign_base(copy_assign_base&& other) = default;
  constexpr copy_assign_base& operator=(copy_assign_base&& other) = default;

  constexpr copy_assign_base& operator=(copy_assign_base const& other) {
    if (this == &other) {
      return *this;
    }
    if (!other.active) {
      this->reset();
      return *this;
    }
    assert(other.active);
    if (this->active) {
      this->value = other.value;
    } else {
      new (&(this->value)) T{other.value};
    }
    this->active = true;
    return *this;
  }
};

template <typename T>
struct copy_assign_base<T, true> : copy_ctor_base<T> {
  using base = copy_ctor_base<T>;
  using base::base;

  constexpr copy_assign_base(const copy_assign_base&) = default;
  constexpr copy_assign_base& operator=(copy_assign_base const&) = default;
  constexpr copy_assign_base(copy_assign_base&& other) = default;
  constexpr copy_assign_base& operator=(copy_assign_base&& other) = default;
};

/*******************************************************************************
 *                        Move construction triviality                         *
 *******************************************************************************/

template <typename T, bool trivial = std::is_trivially_move_constructible_v<T>>
struct move_ctor_base : copy_assign_base<T> {
  using base = copy_assign_base<T>;
  using base::base;
  using base::operator=;

  constexpr move_ctor_base(const move_ctor_base&) = default;
  constexpr move_ctor_base& operator=(move_ctor_base const&) = default;
  constexpr move_ctor_base& operator=(move_ctor_base&&) = default;

  constexpr move_ctor_base(move_ctor_base&& other) : base{} {
    this->active = other.active;
    if (other.active) {
      new (&(this->value)) T{std::move(other.value)};
    }
  }
};

template <typename T>
struct move_ctor_base<T, true> : copy_assign_base<T> {
  using base = copy_assign_base<T>;
  using base::base;
  using base::operator=;

  constexpr move_ctor_base(const move_ctor_base&) = default;
  constexpr move_ctor_base& operator=(move_ctor_base const&) = default;
  constexpr move_ctor_base(move_ctor_base&&) = default;
  constexpr move_ctor_base& operator=(move_ctor_base&&) = default;
};

/*******************************************************************************
 *                         Move assignment triviality                          *
 *******************************************************************************/

template <typename T, bool trivial = std::is_trivially_move_assignable_v<T>&&
                          std::is_trivially_move_constructible_v<T>>
struct move_assign_base : move_ctor_base<T> {
  using base = move_ctor_base<T>;
  using base::base;
  using base::operator=;

  constexpr move_assign_base(const move_assign_base&) = default;
  constexpr move_assign_base& operator=(move_assign_base const&) = default;
  constexpr move_assign_base(move_assign_base&& other) = default;

  constexpr move_assign_base& operator=(move_assign_base&& other) {
    if (this == &other) {
      return *this;
    }
    if (!other.active) {
      this->reset();
      return *this;
    }
    assert(other.active);
    if (this->active) {
      this->value = std::move(other.value);
    } else {
      new (&(this->value)) T{std::move(other.value)};
    }
    this->active = true;
    return *this;
  }
};

template <typename T>
struct move_assign_base<T, true> : move_ctor_base<T> {
  using base = move_ctor_base<T>;
  using base::base;
  using base::operator=;

  constexpr move_assign_base(const move_assign_base&) = default;
  constexpr move_assign_base& operator=(move_assign_base const&) = default;
  constexpr move_assign_base(move_assign_base&& other) = default;
  constexpr move_assign_base& operator=(move_assign_base&& other) = default;
};

} // namespace detail
