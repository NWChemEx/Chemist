#pragma once
#include "../../pychemist.hpp"

namespace chemist::wavefunction {

void export_vector_space(python_module_reference m);
void export_aos(python_module_reference m);

inline void export_wavefunction(python_module_reference m) {
    auto m_wf = m.def_submodule("wavefunction");

    export_vector_space(m_wf);
    export_aos(m_wf);
}

} // namespace chemist::wavefunction
