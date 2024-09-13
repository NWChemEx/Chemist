#pragma once
#include "wavefunction/export_wavefunction.hpp"

namespace chemist {

inline void export_quantum_mechanics(python_module_reference m) {
    wavefunction::export_wavefunction(m);
}

} // namespace chemist
