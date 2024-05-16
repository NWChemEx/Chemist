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
