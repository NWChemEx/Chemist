/*
 * Copyright 2025 NWChemEx-Project
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
#include <chemist/quantum_mechanics/operator/exchange_correlation.hpp>
#include <pybind11/native_enum.h>

namespace chemist::qm_operator {

void export_xc_functionals(python_module_reference m) {
    python_enum_type<xc_functional>(m, "xc_functional", "enum.Enum")
      .value("NONE", xc_functional::NONE)
      .value("CUSTOM", xc_functional::CUSTOM)
      .value("SVWN3", xc_functional::SVWN3)
      .value("SVWN5", xc_functional::SVWN5)
      .value("BLYP", xc_functional::BLYP)
      .value("B3LYP", xc_functional::B3LYP)
      .value("PBE", xc_functional::PBE)
      .value("revPBE", xc_functional::revPBE)
      .value("PBE0", xc_functional::PBE0)
      .finalize();
}

} // namespace chemist::qm_operator
