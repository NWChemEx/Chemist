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

#include "export_braket.hpp"
#include <chemist/quantum_mechanics/braket/braket.hpp>

namespace chemist::braket {

void export_tensor_representation(python_module_reference m) {
    // N.B. This is a stub on the C++ side too. It is exported so that the
    // erased BraKet class and every BraKet<Bra, Op, Ket> instantiation share a
    // Python-visible base, i.e., so isinstance works across both.
    python_class_type<TensorRepresentation, py::smart_holder>(
      m, "TensorRepresentation");
}

} // namespace chemist::braket
