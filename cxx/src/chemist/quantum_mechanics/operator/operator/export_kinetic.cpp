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

namespace chemist::qm_operator {

namespace detail_ {

template<typename T>
void export_kinetic_(const char* name, python_module_reference m) {
    using kinetic_t = Kinetic<T>;
    using op_base_t = OperatorBase;

    auto get_particle = [](const kinetic_t& k) { return k.get_particle(); };
    auto set_particle = [](kinetic_t& k, T& p) { k.set_particle(p); };

    python_class_type<kinetic_t, op_base_t, py::smart_holder>(m, name)
      .def(py::init<>())
      .def(py::init<T>())
      .def(py::self == py::self)
      .def(py::self != py::self)
      .def_property("particle", get_particle, set_particle);
}

} // namespace detail_

void export_kinetic(python_module_reference m) {
    detail_::export_kinetic_<Electron>("KineticElectron", m);
    detail_::export_kinetic_<ManyElectrons>("KineticManyElectrons", m);
    detail_::export_kinetic_<Nucleus>("KineticNuclues", m);
    detail_::export_kinetic_<Nuclei>("KineticNuclei", m);
}

} // namespace chemist::qm_operator
