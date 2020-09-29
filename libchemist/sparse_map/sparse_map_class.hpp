#pragma once
#include "libchemist/sparse_map/detail_/sparse_map_base.hpp"
#include <utility>

namespace libchemist::sparse_map {

/** @brief Stores the sparsity relation among indices.
 *
 */
template<typename IndIndex, typename DepIndex>
class SparseMap :
  public detail_::SparseMapBase<SparseMap<IndIndex, DepIndex>, IndIndex, DepIndex> {
private:
    using my_type    = SparseMap<IndIndex, DepIndex>;
    using base_type  = detail_::SparseMapBase<my_type, IndIndex, DepIndex>;
public:
    // Pull in the base class's ctors
    using detail_::SparseMapBase<my_type, IndIndex, DepIndex>::SparseMapBase;
//    index_set_map indices() const;
//
//    /** @brief Returns the .
//    *
//    *   This is a convenience function for calling indices(key_type, size_type)
//    *   for each dependent index mode.
//    *
//    *   @result A vector of length dep_rank() such that the i-th element
//    *           is the set of non-zero indices for mode i.
//    */
//    index_set_array indices(key_type ind) const;
//
//    /** @brief Returns the set of indices @p ind maps to for a given mode.
//     *
//     *  Particularly when manipulating the full tensor corresponding to the
//     *  dependent indices we need to know what values a given mode will take.
//     *  For independent index @p ind, this function will return the set of valid
//     *  dependent indices for a given mode.
//     *
//     * @param[in] ind The independent index whose dependent indices will be
//     *                considered.
//     * @param[in] mode the mode of each dependent index to be collected.
//     * @return A set of the non-zero indices along mode @p mode for independent
//     *         index @p ind.
//     */
//    index_set indices(key_type ind, size_type mode) const;

}; // class SparseMap

extern template class SparseMap<ElementIndex, ElementIndex>;
extern template class SparseMap<ElementIndex, TileIndex>;
extern template class SparseMap<TileIndex, ElementIndex>;
extern template class SparseMap<TileIndex, TileIndex>;

} // end namespace
