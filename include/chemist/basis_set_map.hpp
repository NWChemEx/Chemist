#pragma once
#include "chemist/basis_set/ao_basis_set.hpp"
#include <utility> //std::pair
#include <vector>

namespace chemist {

// /**
//  * @brief A class for bidirectional mapping between the indices of the AOs,
//  * Shells, and atom centers that comprise a basis set.
//  *
//  * This class iterates through a basis set, filling four vectors that map
//  * indices between corresponding sets of : [atom index -> range of shell
//  * indices], [shell index -> range of ao indices], [ao index -> shell index],
//  * and [shell index -> atom index]. The index of a vector is the LHS map
//  index,
//  * and the element stored at vector.at(LHS) is the corresponding RHS
//  index/index
//  * range. Index ranges hold [index of first element, index of
//  * one-past-the-last-element] for an AO or shell range. Index ranges are
//  * unit-stride.
//  *
//  * [ao index <-> atom index] mappings are retrieved by chaining the [ao index
//  * <-> shell index] and  [shell index <-> atom index] maps.
//  */
// template<typename T>
// class BasisSetMap {
// public:
//     /// The type of an index
//     using size_type = std::size_t;

//     /// The type of an index range.
//     using range = typename std::pair<size_type, size_type>;

//     /**
//      * @brief Makes a BasisSetMap from an AO basis set.
//      *
//      * @param[in] bs The AO basis set to parse for construction of the
//      * BasisSetMap.
//      * @throw std::bad_alloc if there is insufficient memory to allocate the
//      *        BasisSetMap. Strong throw guarantee.
//      */
//     BasisSetMap(const AOBasisSet<T>& bs);

//     /**
//      * @brief Makes an empty BasisSetMap instance.
//      *
//      * @throw std::bad_alloc if there is insufficient memory to allocate the
//      *        BasisSetMap. Strong throw guarantee.
//      */
//     BasisSetMap();

//     /**
//      * @defgroup Copy/Move CTors and Assignment Operators
//      * @brief Can be used to construct BasisSetMaps with state obtained
//      *        from another instance.
//      *
//      * @param[in] rhs The instance to copy/move.  If moved @p rhs is in a
//      valid,
//      *            but otherwise undefined state.
//      * @throw std::bad_alloc copy ctor/assignment operators throw if there is
//      *        insufficient memory to allocate the new state.  Strong throw
//      *        guarantee.  All move functions are no throw guarantee.
//      */
//     ///@{
//     BasisSetMap(const BasisSetMap& rhs);
//     BasisSetMap(BasisSetMap&& rhs) noexcept;
//     BasisSetMap& operator=(const BasisSetMap& rhs);
//     BasisSetMap& operator=(BasisSetMap&& rhs) noexcept;
//     ///@}

//     /// Default dtor
//     ~BasisSetMap() noexcept;

//     /**
//      * @brief Returns an index range for the shells that correspond to a
//      * particular atom center.
//      *
//      * @param idx The index of the atom center, as ordered in the basis set.
//      * @return The range of shell indices that correspond to the specified
//      atom
//      * center index.
//      * @throw std::out_of_range if idx is outside the range of the number of
//      * unique atom centers in the basis set. Strong throw guarantee.
//      */
//     range atom_to_shell(const size_type idx) { return a2s_vec.at(idx); };

//     /**
//      * @brief Returns an index range for the AOs that correspond to a
//      particular
//      * atom center.
//      *
//      * @param idx The index of the atom center, as ordered in the basis set.
//      * @return The range of AO indices that correspond to the specified atom
//      * center index.
//      * @throw std::out_of_range if idx is outside the range of the number of
//      * unique atom centers in the basis set. Strong throw guarantee.
//      */
//     range atom_to_ao(const size_type idx) {
//         auto sr = atom_to_shell(idx);
//         return std::make_pair(s2ao_vec.at(sr.first).first,
//                               s2ao_vec.at(sr.second - 1).second);
//     }

//     /**
//      * @brief Returns an index range for the AOs that correspond to a
//      particular
//      * shell.
//      *
//      * @param idx The index of the shell, as ordered in the basis set.
//      * @return The range of AO indices that correspond to the specified shell
//      * index.
//      * @throw std::out_of_range if idx is outside the range of the number of
//      *        shells in the basis set. Strong throw guarantee.
//      */
//     range shell_to_ao(const size_type idx) { return s2ao_vec.at(idx); }

//     /**
//      * @brief Returns the index of the atom center that corresponds to a
//      * particular shell.
//      *
//      * @param idx The index of the shell, as ordered in the basis set.
//      * @return The index of the atom center that corresponds to the specified
//      * shell index.
//      * @throw std::out_of_range if idx is outside the range of the number of
//      *        shells in the basis set. Strong throw guarantee.
//      */
//     size_type shell_to_atom(const size_type idx) { return s2a_vec.at(idx); }

//     /**
//      * @brief Returns the index of the shell that corresponds to a particular
//      * AO.
//      *
//      * @param idx The index of the AO, as ordered in the basis set.
//      * @return The index of the shell that corresponds to the specified AO
//      * index.
//      * @throw std::out_of_range if idx is outside the range of the number of
//      *        AOs in the basis set. Strong throw guarantee.
//      */
//     size_type ao_to_shell(const size_type idx) { return ao2s_vec.at(idx); }

//     /**
//      * @brief Returns the index of the atom center that corresponds to a
//      * particular AO.
//      *
//      * @param idx The index of the AO, as ordered in the basis set.
//      * @return The index of the atom center that corresponds to the specified
//      * shell index.
//      * @throw std::out_of_range if idx is outside the range of the number of
//      *        AOs in the basis set. Strong throw guarantee.
//      */
//     size_type ao_to_atom(const size_type idx) {
//         const size_type shell_idx = ao_to_shell(idx);
//         return shell_to_atom(shell_idx);
//     }

// private:
//     /// The instance holding the map between [atom index -> shell indices].
//     std::vector<range> a2s_vec;
//     /// The instance holding the map between [shell index -> AO indices].
//     std::vector<range> s2ao_vec;
//     /// The instance holding the map between [AO index -> shell index].
//     std::vector<size_type> ao2s_vec;
//     /// The instance holding the map between [shell index -> atom index].
//     std::vector<size_type> s2a_vec;
// };

// extern template class BasisSetMap<double>;
// extern template class BasisSetMap<float>;

} // namespace chemist