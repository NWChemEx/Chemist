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

void export_contracted_gaussian_view(python_module_reference m) {
    // -- The mutable view ---------------------------------------------------
    auto mutable_view =
      python_class_type<contracted_gaussian_view>(m, "ContractedGaussianView")
        .def(py::init<ContractedGaussian&>(), py::keep_alive<1, 2>())
        .def("as_contracted_gaussian",
             &contracted_gaussian_view::as_contracted_gaussian)
        .def(py::self == py::self)
        .def(py::self != py::self)
        .def("__eq__", [](const contracted_gaussian_view& lhs,
                          const ContractedGaussian& rhs) { return lhs == rhs; })
        .def("__ne__",
             [](const contracted_gaussian_view& lhs,
                const ContractedGaussian& rhs) { return lhs != rhs; });

    detail_::add_contracted_gaussian_readers(mutable_view);
    detail_::add_contracted_gaussian_writers(mutable_view);

    // -- The read-only view --------------------------------------------------
    auto immutable_view =
      python_class_type<const_contracted_gaussian_view>(
        m, "ImmutableContractedGaussianView")
        .def(py::init<const ContractedGaussian&>(), py::keep_alive<1, 2>())
        .def(py::init<const contracted_gaussian_view&>(),
             py::keep_alive<1, 2>())
        .def("as_contracted_gaussian",
             &const_contracted_gaussian_view::as_contracted_gaussian)
        .def(py::self == py::self)
        .def(py::self != py::self)
        .def("__eq__", [](const const_contracted_gaussian_view& lhs,
                          const ContractedGaussian& rhs) { return lhs == rhs; })
        .def("__ne__",
             [](const const_contracted_gaussian_view& lhs,
                const ContractedGaussian& rhs) { return lhs != rhs; });

    detail_::add_contracted_gaussian_readers(immutable_view);

    // A ContractedGaussian and a mutable view are usable wherever a read-only
    // view is, on the C++ side, because both convert to one implicitly.
    // Registering those conversions here gives Python the same behavior.
    py::implicitly_convertible<ContractedGaussian,
                               const_contracted_gaussian_view>();
    py::implicitly_convertible<contracted_gaussian_view,
                               const_contracted_gaussian_view>();
    py::implicitly_convertible<ContractedGaussian, contracted_gaussian_view>();
}

} // namespace chemist::experimental
