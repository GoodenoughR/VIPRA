

#include "parameter_sweep.hpp"

namespace VIPRA {
// NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables) Bug in clang-tidy
MPI_Comm                        ParameterSweep::comm{};
int                             ParameterSweep::rank{};
int                             ParameterSweep::size{};
ParameterSweep::DeferedFinalize ParameterSweep::_finalize{};
// NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables)
}  // namespace VIPRA