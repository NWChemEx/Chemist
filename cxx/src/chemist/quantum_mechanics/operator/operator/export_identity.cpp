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
#include <chemist/quantum_mechanics/operator/identity.hpp>

namespace chemist::qm_operator {

void export_identity(python_module_reference m) {
    using op_base_t = OperatorBase;
    python_class_type<Identity, op_base_t, py::smart_holder>(m, "Identity")
      .def(py::init<>())
      .def(py::self == py::self)
      .def(py::self != py::self);
}

} // namespace chemist::qm_operator
