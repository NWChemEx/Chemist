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
#include <chemist/quantum_mechanics/operator/operator_base.hpp>
#include <pybind11/operators.h>

namespace chemist::qm_operator {

void export_operator_base(python_module_reference m) {
    using base_t = OperatorBase;
    python_class_type<base_t, pybind11::smart_holder>(m, "OperatorBase")
      .def("clone", &base_t::clone)
      .def("are_equal", &base_t::are_equal)
      .def("are_different", &base_t::are_different);
}

} // namespace chemist::qm_operator
