#pragma once

#include <vector>

#include "vipra/geometry/f3d.hpp"

namespace VIPRA {
/**
  * @brief Struct that holds the updated positions and velocities of all pedestrians
  * 
  */
struct State {
  std::vector<f3d> positions;
  std::vector<f3d> velocities;

  [[nodiscard]] auto size() const -> size_t { return positions.size(); }

  void initialize(auto const& pedset)
  {
    positions = pedset.all_coords();
    velocities = pedset.all_velocities();
  }
};
}  // namespace VIPRA