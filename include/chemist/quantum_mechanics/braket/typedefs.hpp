#pragma once
#include <chemist/quantum_mechanics/braket/braket_class.hpp>
#include <chemist/quantum_mechanics/operator/typedefs.hpp>
#include <chemist/quantum_mechanics/wavefunction/aos.hpp>

namespace chemist::braket {

using TElectronAOs = BraKet<AOs, TElectron, AOs>;

};