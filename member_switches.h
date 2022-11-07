#pragma once

/*******************************************************************************
 *                          Copy construction switch                           *
 *******************************************************************************/

template <bool enable>
struct enable_copy_construction {
  constexpr enable_copy_construction() = default;
  constexpr enable_copy_construction(enable_copy_construction&&) = default;
  constexpr enable_copy_construction&
  operator=(enable_copy_construction&& other) = default;
  enable_copy_construction(const enable_copy_construction&) = delete;
  constexpr enable_copy_construction&
  operator=(enable_copy_construction const&) = default;
};

template <>
struct enable_copy_construction<true> {};

/*******************************************************************************
 *                           Copy assignment switch                            *
 *******************************************************************************/

template <bool enable>
struct enable_copy_assignment {
  constexpr enable_copy_assignment() = default;
  constexpr enable_copy_assignment(enable_copy_assignment&&) = default;
  constexpr enable_copy_assignment&
  operator=(enable_copy_assignment&& other) = default;
  enable_copy_assignment(const enable_copy_assignment&) = default;
  constexpr enable_copy_assignment&
  operator=(enable_copy_assignment const&) = delete;
};

template <>
struct enable_copy_assignment<true> {};

/*******************************************************************************
 *                          Move construction switch                           *
 *******************************************************************************/

template <bool enable>
struct enable_move_construction {
  constexpr enable_move_construction() = default;
  constexpr enable_move_construction(const enable_move_construction&) = default;
  constexpr enable_move_construction&
  operator=(enable_move_construction const&) = default;
  enable_move_construction(enable_move_construction&&) = delete;
  constexpr enable_move_construction&
  operator=(enable_move_construction&& other) = default;
};

template <>
struct enable_move_construction<true> {};

/*******************************************************************************
 *                           Move assignment switch                            *
 *******************************************************************************/

template <bool enable>
struct enable_move_assignment {
  constexpr enable_move_assignment() = default;
  constexpr enable_move_assignment(const enable_move_assignment&) = default;
  constexpr enable_move_assignment&
  operator=(enable_move_assignment const&) = default;
  enable_move_assignment(enable_move_assignment&&) = default;
  constexpr enable_move_assignment&
  operator=(enable_move_assignment&& other) = delete;
};

template <>
struct enable_move_assignment<true> {};
