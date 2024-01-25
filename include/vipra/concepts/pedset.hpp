#pragma once

#include <concepts>

#include "vipra/concepts/module.hpp"
#include "vipra/modules.hpp"

#include "vipra/types/f3d.hpp"
#include "vipra/types/idx.hpp"
#include "vipra/types/size.hpp"

namespace VIPRA::Concepts {

// TODO(rolland): add getting distance between pedestrians
// TODO(rolland): add getting direction between pedestrians ?

template <typename pedset_t>
concept can_get_num_peds = requires(const pedset_t pedset) {
  { pedset.num_pedestrians() } -> std::same_as<VIPRA::size>;
};

template <typename pedset_t>
concept can_get_ped_coords = requires(const pedset_t pedset, VIPRA::idx idx) {
  { pedset.ped_coords(idx) } -> std::same_as<VIPRA::f3d>;
  { pedset.all_coords() } -> std::same_as<const std::vector<VIPRA::f3d>&>;
};

template <typename pedset_t>
concept can_get_ped_velocity = requires(const pedset_t pedset, VIPRA::idx idx) {
  { pedset.ped_velocity(idx) } -> std::same_as<VIPRA::f3d>;
  { pedset.all_velocities() } -> std::same_as<const std::vector<VIPRA::f3d>&>;
};

template <typename pedset_t>
concept PedsetModule = is_module<pedset_t, VIPRA::Modules::Type::PEDESTRIANS> && can_get_num_peds<pedset_t> &&
    can_get_ped_coords<pedset_t> && can_get_ped_velocity<pedset_t>;

class DummyPedSet {
  // NOLINTBEGIN
  VIPRA_MODULE_TYPE(PEDESTRIANS);

 public:
  template <typename params_t>
  static void register_params() {}

  void setup(auto& params) {}

  auto num_pedestrians() const -> VIPRA::size { return 1; }
  auto ped_coords(VIPRA::idx /*unused*/) const -> VIPRA::f3d { return VIPRA::f3d{0}; }
  auto all_coords() const -> const std::vector<VIPRA::f3d>& { return _dummy; }
  auto ped_velocity(VIPRA::idx /*unused*/) const -> VIPRA::f3d { return VIPRA::f3d{0}; }
  auto all_velocities() const -> const std::vector<VIPRA::f3d>& { return _dummy; }

 private:
  VIPRA::f3dVec _dummy;
  DummyPedSet() = default;
  // NOLINTEND
};

CHECK_MODULE(PedsetModule, DummyPedSet);
}  // namespace VIPRA::Concepts