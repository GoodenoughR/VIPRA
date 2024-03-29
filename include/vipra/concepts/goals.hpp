#pragma once

#include <concepts>
#include <type_traits>

#include "vipra/concepts/map.hpp"
#include "vipra/concepts/module.hpp"
#include "vipra/concepts/pedset.hpp"
#include "vipra/macros/parameters.hpp"
#include "vipra/modules.hpp"
#include "vipra/random/random.hpp"
#include "vipra/types/time.hpp"

namespace VIPRA::Concepts {

/**
 * @brief Checks that a type can provide goals
 * 
 * @tparam goals_t 
 */
template <typename goals_t>
concept can_get_goals = requires(const goals_t goals) {
  { goals.current_goals() } -> std::same_as<std::vector<VIPRA::f3d> const&>;
  { goals.end_goals() } -> std::same_as<std::vector<VIPRA::f3d> const&>;
  { goals.current_goal(VIPRA::idx{}) } -> std::same_as<VIPRA::f3d const&>;
  { goals.end_goal(VIPRA::idx{}) } -> std::same_as<VIPRA::f3d const&>;
  { goals.is_goal_met(VIPRA::idx{}) } -> std::same_as<bool>;
  { goals.is_sim_goal_met() } -> std::same_as<bool>;
  { goals.time_since_last_goal(VIPRA::idx{}) } -> std::same_as<VIPRA::f_pnt>;
};

/**
 * @brief Checks that a type can be initialized following the goals interface
 * 
 * @tparam goals_t 
 */
template <typename goals_t>
concept can_initialize_goals = requires(goals_t goals, DummyPedSet const& pedset, DummyMap const& map) {
  {goals.initialize(pedset, map)};
};

/**
 * @brief Checks that a type can be updated following the goals interface
 * 
 * @tparam goals_t 
 */
template <typename goals_t>
concept can_update_goals = requires(goals_t goals, DummyPedSet const& pedset, DummyMap const& map) {
  {goals.update(pedset, map, VIPRA::delta_t{})};
  {goals.change_end_goal(VIPRA::idx{}, VIPRA::f3d{})};
};

/**
 * @brief Checks that a type is a goals module
 * 
 * @tparam goals_t 
 */
template <typename goals_t>
concept GoalsModule = is_module<goals_t, VIPRA::Modules::Type::GOALS> && can_initialize_goals<goals_t> &&
    can_get_goals<goals_t> && can_update_goals<goals_t>;

/**
 * @brief Dummy Goals module for use in other concepts
 * 
 */
class DummyGoals {
  // NOLINTBEGIN
 public:
  constexpr static VIPRA::Modules::Type _VIPRA_MODULE_TYPE_ = VIPRA::Modules::Type::GOALS;

  template <VIPRA::Concepts::ParamModule params_t>
  void register_params(params_t& params) {}

  template <VIPRA::Concepts::ParamModule params_t>
  void config(params_t const& params, VIPRA::Random::Engine&) {}

  template <typename pedset_t, typename map_t>
  void initialize(pedset_t const& /*unused*/, map_t const& /*unused*/) {}

  template <typename pedset_t, typename map_t>
  void update(pedset_t const& /*unused*/, map_t const& /*unused*/, VIPRA::f_pnt) {}

  void change_end_goal(VIPRA::idx /*unused*/, VIPRA::f3d /*unused*/) {}

  auto current_goals() const -> const VIPRA::f3dVec& { return _dummy; }
  auto end_goals() const -> const VIPRA::f3dVec& { return _dummy; }
  auto current_goal(VIPRA::idx) const -> VIPRA::f3d const& { return _dummy2; }
  auto end_goal(VIPRA::idx) const -> VIPRA::f3d const& { return _dummy2; }
  auto is_goal_met(VIPRA::idx) const -> bool { return false; }
  auto is_sim_goal_met() const -> bool { return false; }
  auto time_since_last_goal(VIPRA::idx) const -> VIPRA::f_pnt { return 0.0; }

 private:
  VIPRA::f3dVec _dummy;
  VIPRA::f3d    _dummy2;
  // NOLINTEND
};

CHECK_MODULE(GoalsModule, Concepts::DummyGoals);
}  // namespace VIPRA::Concepts
