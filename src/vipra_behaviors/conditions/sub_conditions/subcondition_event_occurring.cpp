
#include "conditions/subconditions/subcondition_event_occurring.hpp"

namespace VIPRA::Behaviors {

void SubConditionEventOccurring::operator()(Simpack pack, const VIPRA::idxVec& /*unused*/,
                                            std::vector<Target> const& /*unused*/, std::vector<bool>& met,
                                            std::vector<bool> const& /*unused*/, BoolOp /*unused*/) const {
  std::fill(met.begin(), met.end(), pack.get_context().events[_event].is_occurring());
}
}  // namespace VIPRA::Behaviors