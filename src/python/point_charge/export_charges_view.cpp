/*
 * Copyright 2024 NWChemEx-Project
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

#include "export_point_charge.hpp"
#include <chemist/point_charge/charges_view.hpp>
#include <pybind11/operators.h>
#include <sstream>

namespace chemist {
namespace detail_ {

template<typename T>
void export_charges_view_(const char* name, python_module_reference m) {
    using view_type           = ChargesView<Charges<T>>;
    using charges_type        = typename view_type::charges_type;
    using reference           = view_type&;
    using size_type           = typename view_type::size_type;
    using point_set_reference = typename view_type::point_set_reference;

    auto get_point_set_fxn = [](reference self) { return self.point_set(); };
    auto set_point_set_fxn = [](reference self, point_set_reference points) {
        self.point_set() = points;
    };

    auto str_fxn = [](const view_type& charges) {
        std::ostringstream stream;
        stream << charges;
        return stream.str();
    };

    pybind11::keep_alive<0, 1> ka;

    python_class_type<view_type>(m, name)
      .def(pybind11::init<>())
      .def(pybind11::init<charges_type&>())
      .def_property(
        "point_set",
        pybind11::cpp_function(
          get_point_set_fxn, pybind11::return_value_policy::take_ownership, ka),
        pybind11::cpp_function(set_point_set_fxn))
      .def("empty", [](reference self) { return self.empty(); })
      .def("at", [](reference self, size_type i) { return self[i]; })
      .def("size", [](reference self) { return self.size(); })
      .def("__str__", str_fxn)
      .def(pybind11::self == pybind11::self)
      .def(pybind11::self == charges_type())
      .def(pybind11::self != pybind11::self);
}

} // namespace detail_

void export_charges_view(python_module_reference m) {
    detail_::export_charges_view_<float>("ChargesViewF", m);
    detail_::export_charges_view_<double>("ChargesViewD", m);
}

} // namespace chemist
