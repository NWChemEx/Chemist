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
#include <string>
#include <vector>

namespace chemist::experimental {

void export_atomic_basis_set_class(python_module_reference m) {
    using size_type = std::size_t;

    py::enum_<ShellPurity>(m, "ShellPurity")
      .value("cartesian", ShellPurity::cartesian)
      .value("pure", ShellPurity::pure);

    py::enum_<AOOrdering>(m, "AOOrdering").value("cca", AOOrdering::cca);

    auto c =
      python_class_type<AtomicBasisSet>(m, "AtomicBasisSet")
        .def(py::init<>())
        .def(py::init<std::string, size_type, Point, ShellPurity, AOOrdering>(),
             py::arg("name"), py::arg("atomic_number"), py::arg("center"),
             py::arg("purity")   = ShellPurity::cartesian,
             py::arg("ordering") = AOOrdering::cca)
        // Narrows the templated add_shell to double; see CartesianAO.
        .def("add_shell",
             [](AtomicBasisSet& abs, size_type l, std::vector<double> cs,
                std::vector<double> es) {
                 abs.add_shell(l, cs.begin(), cs.end(), es.begin(), es.end());
             })
        .def("push_back", [](AtomicBasisSet& abs,
                             const AOShell& shell) { abs.push_back(shell); })
        .def("push_back",
             [](AtomicBasisSet& abs, const AOShellView& shell) {
                 abs.push_back(shell);
             })
        .def("swap", &AtomicBasisSet::swap)
        .def(py::self == py::self)
        .def(py::self != py::self);

    detail_::add_atomic_basis_set_readers(c);
    detail_::add_atomic_basis_set_writers(c);
}

} // namespace chemist::experimental
