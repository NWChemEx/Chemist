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

#include "../export_experimental.hpp"
#include <vector>

namespace chemist::experimental {

void export_molecular_basis_set_class(python_module_reference m) {
    auto c =
      python_class_type<MolecularBasisSet>(m, "MolecularBasisSet")
        .def(py::init<>())
        .def(py::init([](const std::vector<AtomicBasisSet>& atoms) {
                 return MolecularBasisSet(atoms.begin(), atoms.end());
             }),
             py::arg("atoms"))
        // AtomicBasisSet and both of its views convert to the read-only view
        .def(
          "push_back",
          [](MolecularBasisSet& mbs, const const_atomic_basis_set_view& atom) {
              mbs.push_back(atom);
          })
        .def("swap", &MolecularBasisSet::swap)
        .def(py::self == py::self)
        .def(py::self != py::self);

    detail_::add_molecular_basis_set_readers(c);
    detail_::add_molecular_basis_set_writers(c);
}

} // namespace chemist::experimental
