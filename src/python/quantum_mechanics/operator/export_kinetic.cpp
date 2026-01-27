/*
 * Copyright 2026 NWChemEx-Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "export_operator.hpp"
#include <chemist/quantum_mechanics/operator/kinetic.hpp>
#include <pybind11/operators.h>

namespace chemist::qm_operator {

template<typename T>
void export_kinetic_(const char* name, python_module_reference m) {
    using kinetic_t = Kinetic<T>;

    auto get_particle = [](const kinetic_t& k) { return k.particle(); };
    auto set_particle = [](kinetic_t& k, T& p) { k.particle() = p; };

    python_class_type<kinetic_t>(m, name)
      .def(pybind11::init<>())
      .def(pybind11::init<T>())
      .def(pybind11::self == pybind11::self)
      .def(pybind11::self != pybind11::self)
      .def_property("particle", get_particle, set_particle);
}

void export_kinetic(python_module_reference m) {
    export_kinetic_<Electron>("KineticElectron", m);
    export_kinetic_<ManyElectrons>("KineticManyElectrons", m);
    export_kinetic_<Nucleus>("KineticNuclues", m);
    export_kinetic_<Nuclei>("KineticNuclei", m);
}

} // namespace chemist::qm_operator
