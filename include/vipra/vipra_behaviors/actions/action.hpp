#ifndef ACTION_HPP
#define ACTION_HPP

#include <optional>

#include "vipra/geometry/f3d.hpp"

#include "vipra/vipra_behaviors/definitions/behavior_context.hpp"
#include "vipra/vipra_behaviors/definitions/sim_pack.hpp"

#include "vipra/vipra_behaviors/actions/atom.hpp"
#include "vipra/vipra_behaviors/conditions/condition.hpp"

#include "vipra/vipra_behaviors/targets/target_selector.hpp"
#include "vipra/vipra_behaviors/targets/target_selectors/target_self.hpp"

#include "vipra/vipra_behaviors/time/time.hpp"

#include "vipra/vipra_behaviors/util/class_types.hpp"
#include "vipra/vipra_behaviors/util/timed_latch.hpp"

#include "vipra/vipra_behaviors/values/numeric_value.hpp"

namespace VIPRA::Behaviors {
template <typename atom_t, typename condition_t, typename target_t>
class Action {
  DEFAULT_CONSTRUCTIBLE(Action)
  COPYABLE(Action)
  MOVEABLE(Action)

 public:
  void initialize(auto pack);

  void perform_action(auto pack, VIPRA::idxVec&, std::vector<Target> const&);
  void perform_action(auto pack, const VIPRA::idxVec&, std::vector<bool> const&, std::vector<Target> const&);

  void add_condition(condition_t const&);
  void add_atom(atom_t const&);
  void add_duration(Behaviors::NumericValue const&);
  void add_target(target_t&&);

  [[nodiscard]] auto has_condition() -> bool { return _condition.has_value(); }
  [[nodiscard]] auto condition() -> std::optional<condition_t>& { return _condition; }

  [[nodiscard]] auto has_duration() -> bool { return _duration.has_value(); }
  [[nodiscard]] auto duration() -> std::optional<TimedLatchCollection>& { return _duration; }

  [[nodiscard]] auto has_target() -> bool { return _targets.has_value(); }
  [[nodiscard]] auto targets() -> std::optional<target_t>& { return _targets; }

 private:
  std::vector<atom_t>                 _atoms;
  std::optional<condition_t>          _condition;
  std::optional<TimedLatchCollection> _duration;
  std::optional<target_t>             _targets;
};

}  // namespace VIPRA::Behaviors

#endif