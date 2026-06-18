/*
 * Copyright 2024 NWChemEx
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

#include "export_nucleus.hpp"
#include <chemist/nucleus/nuclei_view.hpp>
#include <sstream>

namespace chemist {
namespace detail_ {

template<typename NucleiType>
void export_nuclei_view_(const char* name, python_module_reference m) {
    // N.B. We don't want to export the base class so we use lambdas to
    //      call the methods.

    using nuclei_type = NucleiType;
    using view_type   = NucleiView<nuclei_type>;
    using reference   = view_type&;
    using size_type   = typename nuclei_type::size_type;
    using other_nuclei_type =
      std::conditional_t<std::is_const_v<nuclei_type>, nuclei_type,
                         const nuclei_type>;
    using other_view_type = NucleiView<other_nuclei_type>;

    python_class_type<view_type>(m, name)
      .def(py::init<>())
      .def(py::init<nuclei_type&>())
      .def("empty", [](reference self) { return self.empty(); })
      .def("at", [](reference self, size_type i) { return self[i]; })
      .def("size", [](reference self) { return self.size(); })
      .def("as_nuclei", &view_type::as_nuclei)
      .def("__str__",
           [](reference nuc) {
               std::ostringstream stream;
               stream << nuc;
               return stream.str();
           })
      .def(
        "__iter__",
        [](reference self) {
            return py::make_iterator(self.begin(), self.end());
        },
        py::keep_alive<0, 1>())
      .def(py::self == py::self)
      .def(py::self == nuclei_type{})
      .def(nuclei_type{} == py::self)
      .def(py::self == other_view_type{})
      .def(py::self != py::self)
      .def(py::self != nuclei_type{})
      .def(nuclei_type{} != py::self)
      .def(py::self != other_view_type{});
}

} // namespace detail_

void export_nuclei_view(python_module_reference m) {
    detail_::export_nuclei_view_<Nuclei>("NucleiView", m);
    detail_::export_nuclei_view_<const Nuclei>("ImmutableNucleiView", m);
}

} // namespace chemist
