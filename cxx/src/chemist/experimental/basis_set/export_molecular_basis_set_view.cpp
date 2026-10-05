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

namespace chemist::experimental {

void export_molecular_basis_set_view(python_module_reference m) {
    using view_type       = molecular_basis_set_view;
    using const_view_type = const_molecular_basis_set_view;

    // -- The mutable view ---------------------------------------------------
    auto mutable_view =
      python_class_type<view_type>(m, "MolecularBasisSetView")
        .def(py::init<MolecularBasisSet&>(), py::keep_alive<1, 2>())
        .def("as_molecular_basis_set", &view_type::as_molecular_basis_set)
        .def("swap", &view_type::swap)
        .def(py::self == py::self)
        .def(py::self != py::self);

    detail_::add_comparisons_with<MolecularBasisSet>(mutable_view);
    detail_::add_molecular_basis_set_readers(mutable_view);
    detail_::add_molecular_basis_set_writers(mutable_view);

    // -- The read-only view --------------------------------------------------
    auto immutable_view =
      python_class_type<const_view_type>(m, "ImmutableMolecularBasisSetView")
        .def(py::init<const MolecularBasisSet&>(), py::keep_alive<1, 2>())
        .def(py::init<const view_type&>(), py::keep_alive<1, 2>())
        .def("as_molecular_basis_set", &const_view_type::as_molecular_basis_set)
        .def("swap", &const_view_type::swap)
        .def(py::self == py::self)
        .def(py::self != py::self);

    detail_::add_comparisons_with<MolecularBasisSet>(immutable_view);
    detail_::add_comparisons_with<view_type>(immutable_view);
    detail_::add_molecular_basis_set_readers(immutable_view);

    // Mirrors the implicit conversions the C++ classes provide.
    py::implicitly_convertible<MolecularBasisSet, const_view_type>();
    py::implicitly_convertible<view_type, const_view_type>();
    py::implicitly_convertible<MolecularBasisSet, view_type>();
}

} // namespace chemist::experimental
