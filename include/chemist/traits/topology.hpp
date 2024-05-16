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

#pragma once
#include <array>
#include <chemist/traits/chemist_class_traits.hpp>
#include <iostream>
#include <memory>
#include <set>
#include <vector>
namespace chemist {
namespace topology {
class ConnectivityTable;
} // namespace topology

template<>
struct ChemistClassTraits<topology::ConnectivityTable> {
    /// Type used for indexing atoms and bonds
    using size_type = std::size_t;

    /// Type used to store the atom indices in a bond
    using offset_pair = std::array<size_type, 2>;

    /// Type of a container of offset_pair objects
    using offset_pair_list = std::vector<offset_pair>;

    /// Type of a set-like container of atom indices
    using atom_indices_set = std::set<size_type>;
};

} // namespace chemist
