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

void export_ao(python_module_reference m) {
    // AO and AOView are abstract, so neither gets a ctor. They are exposed so
    // that, as in C++, every concrete AO shares a base, and so that clone and
    // as_ao can hand back whichever concrete AO the base actually holds:
    // pybind11 downcasts a returned std::unique_ptr<AO> to the most-derived
    // registered type.

    // -- AO -----------------------------------------------------------------
    auto ao = python_class_type<AO>(m, "AO")
                .def("clone", &AO::clone)
                .def("are_equal", &AO::are_equal)
                .def("are_different", &AO::are_different);

    detail_::add_ao_readers(ao);

    // -- AOView -------------------------------------------------------------
    auto ao_view = python_class_type<AOView>(m, "AOView")
                     // The clone aliases what *this aliases, so it must keep
                     // *this (and through it, the aliased AO) alive.
                     .def("clone", &AOView::clone, py::keep_alive<0, 1>())
                     .def("as_ao", &AOView::as_ao)
                     .def("are_equal", &AOView::are_equal)
                     .def("are_different", &AOView::are_different);

    detail_::add_ao_readers(ao_view);
}

} // namespace chemist::experimental
