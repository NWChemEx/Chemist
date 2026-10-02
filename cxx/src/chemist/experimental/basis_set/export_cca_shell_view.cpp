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
namespace {

/// Binds the mutable and read-only views of CCAShell<AOType>
template<typename AOType>
void export_cca_shell_views(python_module_reference m, const char* name,
                            const char* immutable_name) {
    using shell_type      = CCAShell<AOType>;
    using view_type       = CCAShellView<shell_type>;
    using const_view_type = CCAShellView<const shell_type>;

    // -- The mutable view ---------------------------------------------------
    auto mutable_view =
      python_class_type<view_type, AOShellView>(m, name)
        .def(py::init<shell_type&>(), py::keep_alive<1, 2>())
        .def(py::init<contracted_gaussian_view>(), py::keep_alive<1, 2>())
        .def("as_cca_shell", &view_type::as_cca_shell)
        .def(py::self == py::self)
        .def(py::self != py::self);

    detail_::add_comparisons_with<shell_type>(mutable_view);
    detail_::add_cca_shell_readers(mutable_view);
    detail_::add_cca_shell_writers(mutable_view);

    // -- The read-only view --------------------------------------------------
    auto immutable_view =
      python_class_type<const_view_type, AOShellView>(m, immutable_name)
        .def(py::init<const shell_type&>(), py::keep_alive<1, 2>())
        .def(py::init<const view_type&>(), py::keep_alive<1, 2>())
        .def(py::init<const_contracted_gaussian_view>(), py::keep_alive<1, 2>())
        .def("as_cca_shell", &const_view_type::as_cca_shell)
        .def(py::self == py::self)
        .def(py::self != py::self);

    detail_::add_comparisons_with<shell_type>(immutable_view);
    detail_::add_cca_shell_readers(immutable_view);

    // Mirrors the implicit conversions the C++ classes provide.
    py::implicitly_convertible<shell_type, const_view_type>();
    py::implicitly_convertible<view_type, const_view_type>();
    py::implicitly_convertible<shell_type, view_type>();
}

} // namespace

void export_cca_shell_view(python_module_reference m) {
    export_cca_shell_views<CartesianAO>(m, "CartesianCCAShellView",
                                        "ImmutableCartesianCCAShellView");
    export_cca_shell_views<SphericalAO>(m, "SphericalCCAShellView",
                                        "ImmutableSphericalCCAShellView");
}

} // namespace chemist::experimental
