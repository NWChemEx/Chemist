#pragma once
#include <chemist/quantum_mechanics/operator/kinetic.hpp>

namespace chemist::qm_operator {

using t_e_type = Kinetic<Electron>;
using T_e_type = Kinetic<ManyElectrons>;
using t_n_type = Kinetic<Nucleus>;
using T_n_type = Kinetic<Nuclei>;

} // namespace chemist::qm_operator