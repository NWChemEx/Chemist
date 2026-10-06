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

/** @brief Binds indexing for the abstract shells.
 *
 *  AOShellBase::at returns a pointer to a newly built AOView, which pybind11
 *  takes ownership of and downcasts to the concrete view type. The view
 *  aliases the shell, so the shell is kept alive for as long as the view is.
 */
template<typename PyClass>
void add_base_indexing(PyClass& c) {
    using class_type = typename PyClass::type;
    auto at = [](const class_type& s, std::size_t i) { return s.at(i); };
    c.def("at", at, py::keep_alive<0, 1>())
      .def("__getitem__", at, py::keep_alive<0, 1>());
}

/// Binds AOShell<AOType> as @p name and AOShellView<AOType> as @p view_name
template<typename AOType>
void export_typed_ao_shells(python_module_reference m, const char* name,
                            const char* view_name) {
    using shell_type      = AOShell<AOType>;
    using shell_view_type = AOShellView<AOType>;

    auto shell =
      python_class_type<shell_type, AOShellBase>(m, name)
        .def("clone", &shell_type::clone)
        .def("as_view", &shell_type::as_view, py::keep_alive<0, 1>());

    detail_::add_typed_ao_shell_readers(shell);

    auto shell_view =
      python_class_type<shell_view_type, AOShellBaseView>(m, view_name)
        .def("clone", &shell_view_type::clone, py::keep_alive<0, 1>())
        .def("as_shell", &shell_view_type::as_shell);

    detail_::add_typed_ao_shell_readers(shell_view);
}

/** @brief Binds as_cartesian_shell or as_spherical_shell as @p name.
 *
 *  The result is the argument, downcast, so pybind11 finds and hands back the
 *  Python object already wrapping it. Hence the reference policy, and no
 *  keep_alive: the result would only be keeping itself alive.
 */
template<typename ShellCaster, typename ViewCaster>
void export_shell_cast(python_module_reference m, const char* name,
                       ShellCaster shell_cast, ViewCaster view_cast) {
    m.def(name, shell_cast, py::return_value_policy::reference)
      .def(name, view_cast, py::return_value_policy::reference);
}

} // namespace

void export_ao_shell(python_module_reference m) {
    // Abstract, for the same reasons as AO and AOView; see export_ao.

    // -- AOShellBase --------------------------------------------------------
    auto shell =
      python_class_type<AOShellBase>(m, "AOShellBase")
        .def("clone", &AOShellBase::clone)
        .def("as_view", &AOShellBase::as_view, py::keep_alive<0, 1>())
        .def("are_equal", &AOShellBase::are_equal)
        .def("are_different", &AOShellBase::are_different);

    detail_::add_ao_shell_readers(shell);
    add_base_indexing(shell);

    // -- AOShellBaseView ----------------------------------------------------
    auto shell_view =
      python_class_type<AOShellBaseView>(m, "AOShellBaseView")
        .def("clone", &AOShellBaseView::clone, py::keep_alive<0, 1>())
        .def("as_shell", &AOShellBaseView::as_shell)
        .def("are_equal", &AOShellBaseView::are_equal)
        .def("are_different", &AOShellBaseView::are_different);

    detail_::add_ao_shell_readers(shell_view);
    add_base_indexing(shell_view);

    // -- AOShell<AOType> and AOShellView<AOType> ------------------------------
    export_typed_ao_shells<CartesianAO>(m, "CartesianAOShell",
                                        "CartesianAOShellView");
    export_typed_ao_shells<SphericalAO>(m, "SphericalAOShell",
                                        "SphericalAOShellView");

    // -- Downcasts ------------------------------------------------------------
    using base_shell_ref = const AOShellBase&;
    using base_view_ref  = const AOShellBaseView&;
    export_shell_cast(
      m, "as_cartesian_shell",
      [](base_shell_ref s) -> decltype(auto) { return as_cartesian_shell(s); },
      [](base_view_ref s) -> decltype(auto) { return as_cartesian_shell(s); });
    export_shell_cast(
      m, "as_spherical_shell",
      [](base_shell_ref s) -> decltype(auto) { return as_spherical_shell(s); },
      [](base_view_ref s) -> decltype(auto) { return as_spherical_shell(s); });
}

} // namespace chemist::experimental
