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
 *  AOShell::at returns a pointer to a newly built AOView, which pybind11 takes
 *  ownership of and downcasts to the concrete view type. The view aliases the
 *  shell, so the shell is kept alive for as long as the view is.
 */
template<typename PyClass>
void add_base_indexing(PyClass& c) {
    using class_type = typename PyClass::type;
    auto at = [](const class_type& s, std::size_t i) { return s.at(i); };
    c.def("at", at, py::keep_alive<0, 1>())
      .def("__getitem__", at, py::keep_alive<0, 1>());
}

} // namespace

void export_ao_shell(python_module_reference m) {
    // Abstract, for the same reasons as AO and AOView; see export_ao.

    // -- AOShell ------------------------------------------------------------
    auto shell = python_class_type<AOShell>(m, "AOShell")
                   .def("clone", &AOShell::clone)
                   .def("as_view", &AOShell::as_view, py::keep_alive<0, 1>())
                   .def("are_equal", &AOShell::are_equal)
                   .def("are_different", &AOShell::are_different);

    detail_::add_ao_shell_readers(shell);
    add_base_indexing(shell);

    // -- AOShellView --------------------------------------------------------
    auto shell_view =
      python_class_type<AOShellView>(m, "AOShellView")
        .def("clone", &AOShellView::clone, py::keep_alive<0, 1>())
        .def("as_shell", &AOShellView::as_shell)
        .def("are_equal", &AOShellView::are_equal)
        .def("are_different", &AOShellView::are_different);

    detail_::add_ao_shell_readers(shell_view);
    add_base_indexing(shell_view);
}

} // namespace chemist::experimental
