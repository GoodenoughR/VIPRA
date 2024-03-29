#pragma once

#include <concepts>
#include <utility>

#include "vipra/concepts/goals.hpp"
#include "vipra/concepts/map.hpp"
#include "vipra/concepts/obstacle_set.hpp"
#include "vipra/concepts/output.hpp"
#include "vipra/concepts/output_coordinator.hpp"
#include "vipra/concepts/pedset.hpp"

#include "vipra/modules.hpp"

#include "vipra/random/random.hpp"

#include "vipra/types/state.hpp"
#include "vipra/types/time.hpp"

namespace VIPRA::Concepts {

/**
 * @brief Checks that a type can be initialized following the model module interface
 * 
 * @tparam model_t 
 */
template <typename model_t>
concept can_initialize_model = requires(model_t model, DummyPedSet const& pedset, DummyMap const& map,
                                        DummyGoals const& goals, DummyOutput& output,
                                        VIPRA::Random::Engine& engine) {
  {model.initialize(pedset, map, goals, output, engine)};
};

/**
 * @brief Checks that a type has a pedestrian dynamics timestep method
 * 
 * @tparam model_t 
 */
template <typename model_t>
concept has_model_timestep = requires(model_t model, DummyPedSet const& pedset, DummyMap const& map,
                                      DummyGoals const& goals, DummyOutput& output, VIPRA::State& state,
                                      VIPRA::delta_t deltaT, VIPRA::timestep timestep) {
  { model.timestep(pedset, map, goals, output, state, deltaT, timestep) } -> std::same_as<void>;
};

/**
 * @brief Checks that a type is a model module
 * 
 * @tparam model_t 
 */
template <typename model_t>
concept ModelModule = is_module<model_t, VIPRA::Modules::Type::MODEL> && has_model_timestep<model_t> &&
    can_initialize_model<model_t>;

/**
 * @brief Dummy model for use in other concepts
 * 
 */
class DummyModel {
  // NOLINTBEGIN
 public:
  constexpr static VIPRA::Modules::Type _VIPRA_MODULE_TYPE_ = VIPRA::Modules::Type::MODEL;

  template <typename params_t>
  void register_params(params_t&) {}

  template <typename pedset_t, typename map_t, typename goals_t, typename output_t>
  void initialize(pedset_t const&, map_t const&, goals_t const&, output_t&, VIPRA::Random::Engine&) {}

  void config(auto&, VIPRA::Random::Engine&) {}

  void timestep(DummyPedSet const&, DummyMap const&, DummyGoals const&, DummyOutput const&, VIPRA::State&,
                VIPRA::delta_t, VIPRA::timestep) {}

 private:
  VIPRA::State _state;
  // NOLINTEND
};

CHECK_MODULE(ModelModule, DummyModel);

}  // namespace VIPRA::Concepts