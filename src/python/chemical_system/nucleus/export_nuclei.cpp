/*
 * Copyright 2023 NWChemEx-Project
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
#include <chemist/chemical_system/nucleus/nuclei.hpp>
#include <pybind11/operators.h>
#include <sstream>

namespace chemist {

void export_nuclei(python_module_reference m) {
    // N.B. We don't want to export the base class so we use lambdas to
    //      call the methods.

    using nuclei_type      = Nuclei;
    using nuclei_reference = nuclei_type&;
    using charges_type     = typename nuclei_type::charge_set_type;
    using value_type       = typename nuclei_type::value_type;
    using size_type        = typename nuclei_type::size_type;

    // Factor long functions out for readability
    //{
    auto list_ctor = [](pybind11::list nuclei) {
        std::vector<value_type> values;
        for(auto& nucleus_py : nuclei) {
            auto nucleus_i = nucleus_py.cast<value_type>();
            values.push_back(nucleus_i);
        }
        return nuclei_type(values.begin(), values.end());
    };

    auto get_charges_fxn = [](nuclei_reference self) { return self.charges(); };
    auto set_charges_fxn = [](nuclei_reference self, charges_type charges) {
        self.charges() = charges;
    };

    auto push_back = [](nuclei_reference self, value_type v) {
        return self.push_back(std::move(v));
    };
    auto str_fxn = [](const chemist::Nuclei& nuc) {
        std::ostringstream stream;
        stream << nuc;
        return stream.str();
    };
    auto iter_fxn = [](nuclei_reference self) {
        return pybind11::make_iterator(self.begin(), self.end());
    };
    //}

    python_class_type<nuclei_type>(m, "Nuclei")
      .def(pybind11::init<>())
      .def(pybind11::init(list_ctor))
      .def_property("charges", get_charges_fxn, set_charges_fxn)
      .def("empty", [](nuclei_reference self) { return self.empty(); })
      .def("push_back", push_back)
      .def("at", [](nuclei_reference self, size_type i) { return self[i]; })
      .def("size", [](nuclei_reference self) { return self.size(); })
      .def("__repr__", str_fxn)
      .def("__str__", str_fxn)
      .def("__iter__", iter_fxn, pybind11::keep_alive<0, 1>())
      .def(pybind11::self == pybind11::self)
      .def(pybind11::self != pybind11::self);
}

} // namespace chemist
