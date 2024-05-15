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

#include "export_fragmenting.hpp"
#include <chemist/fragmenting/fragmented_nuclei.hpp>
#include <pybind11/operators.h>

namespace chemist::fragmenting {

void export_fragmented_nuclei(python_module_reference m) {
    using fragmented_nuclei_type = FragmentedNuclei<Nuclei>;
    using supersystem_type = typename fragmented_nuclei_type::supersystem_type;
    using cap_set_type     = typename fragmented_nuclei_type::cap_set_type;
    using reference        = fragmented_nuclei_type&;
    using size_type        = typename fragmented_nuclei_type::size_type;
    using nucleus_map_type = typename fragmented_nuclei_type::nucleus_map_type;
    using nucleus_index_set =
      typename fragmented_nuclei_type::nucleus_index_set;
    using nucleus_type      = typename supersystem_type::value_type;
    using nucleus_reference = typename supersystem_type::reference;

    pybind11::keep_alive<0, 1> ka;

    auto value_ctor = [](supersystem_type ss, pybind11::list frags,
                         cap_set_type caps) {
        nucleus_map_type indices;
        for(auto& frag_indices : frags) {
            indices.push_back(pybind_to_buffer<size_type>(frag_indices));
        }
        return fragmented_nuclei_type(std::move(ss), std::move(indices),
                                      std::move(caps));
    };

    auto insert_index = [](reference self, pybind11::list indices_py) {
        if(indices_py.size() == 0) {
            self.insert(nucleus_index_set{});
            return;
        }
        auto elem0 = indices_py[0];
        if(can_cast<size_type>(elem0)) {
            self.insert(pybind_to_buffer<size_type>(indices_py));
        } else if(can_cast<nucleus_type>(elem0)) {
            auto b = pybind_to_buffer<nucleus_type>(indices_py);
            self.insert(b.begin(), b.end());
        } else if(can_cast<nucleus_reference>(elem0)) {
            auto b = pybind_to_buffer<nucleus_reference>(indices_py);
            self.insert(b.begin(), b.end());
        } else {
            throw std::runtime_error("Can't convert input list");
        }
    };
    auto cap_set     = [](reference self) { return self.cap_set(); };
    auto supersystem = [](reference self) { return self.supersystem(); };
    auto at_fxn      = [](reference self, size_type i) { return self.at(i); };
    auto iter_fxn    = [](reference self) {
        return pybind11::make_iterator(self.begin(), self.end());
    };

    python_class_type<fragmented_nuclei_type>(m, "FragmentedNuclei")
      .def(pybind11::init<>())
      .def(pybind11::init<supersystem_type>())
      .def(pybind11::init(value_ctor), pybind11::arg("supersystem"),
           pybind11::arg("nucleus_to_fragment"),
           pybind11::arg("caps") = cap_set_type{})
      .def("insert", insert_index)
      .def("cap_set", cap_set, ka)
      .def("add_cap", &fragmented_nuclei_type::add_cap)
      .def("supersystem", supersystem, ka)
      .def("empty", [](reference self) { return self.empty(); })
      .def("at", at_fxn, ka)
      .def("size", [](reference self) { return self.size(); })
      .def("__iter__", iter_fxn, ka)
      .def(pybind11::self == pybind11::self)
      .def(pybind11::self != pybind11::self);
}

} // namespace chemist::fragmenting
