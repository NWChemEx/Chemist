#pragma once
#include "../../pychemist.hpp"

namespace chemist {

void export_electron_class(python_module_reference m);
void export_many_electrons(python_module_reference m);

inline void export_electron(python_module_reference m) {
    export_electron_class(m);
    export_many_electrons(m);
}

} // namespace chemist