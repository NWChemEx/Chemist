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

#include "export_wavefunction.hpp"
#include <chemist/quantum_mechanics/wavefunction/cmos.hpp>
#include <chemist/quantum_mechanics/wavefunction/determinant.hpp>
#include <chemist/quantum_mechanics/wavefunction/mos.hpp>

namespace chemist::wavefunction {

namespace detail_ {

template<typename T>
void export_determinant_(const char* name, python_module_reference m) {
    using determinant_t = Determinant<T>;
    using index_set_t   = typename determinant_t::orbital_index_set_type;
    using size_type     = typename determinant_t::size_type;

    // N.B. orbital_index_set_type is a boost::container::flat_set, which
    // pybind11/stl.h does not know how to cast (its set_caster only covers
    // std::set and std::unordered_set). Take the occupied orbitals as a
    // sequence of indices and build the flat_set on this side instead.
    using index_vector_t = std::vector<size_type>;

    auto value_ctor = [](index_vector_t occupied, T orbitals) {
        index_set_t occ(occupied.begin(), occupied.end());
        return determinant_t(std::move(occ), std::move(orbitals));
    };

    auto get_indices = [](const determinant_t& self) {
        const auto& occ = self.orbital_indices();
        return index_vector_t(occ.begin(), occ.end());
    };
    auto set_indices = [](determinant_t& self, index_vector_t occupied) {
        self.orbital_indices() = index_set_t(occupied.begin(), occupied.end());
    };
    auto get_orbitals = [](const determinant_t& self) {
        return self.orbitals();
    };
    auto set_orbitals = [](determinant_t& self, T& orbitals) {
        self.orbitals() = orbitals;
    };
    auto occupations = [](const determinant_t& self) {
        return self.occupations();
    };

    python_class_type<determinant_t, Wavefunction>(m, name)
      .def(py::init<>())
      .def(py::init(value_ctor))
      .def_property("orbital_indices", get_indices, set_indices)
      .def_property("orbitals", get_orbitals, set_orbitals)
      .def("occupations", occupations)
      .def(py::self == py::self)
      .def(py::self != py::self);
}

} // namespace detail_

void export_determinant(python_module_reference m) {
    detail_::export_determinant_<MOs>("DeterminantMOs", m);
    detail_::export_determinant_<CMOs>("DeterminantCMOs", m);
}

} // namespace chemist::wavefunction
