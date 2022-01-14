#pragma once
#include "chemist/tensor/shapes/sparse_shape.hpp"
#include "shape_pimpl.hpp"

namespace chemist::tensor::detail_ {

/** @brief Implements SparseShape class
 *
 *  @tparam FieldType Type of the field in the associated tensor. Assumed to be
 *                    either field::Scalar or field::Tensor.
 */
template<typename FieldType>
class SparseShapePIMPL : public ShapePIMPL<FieldType> {
private:
    /// Type of the Shape this PIMPL implements
    using parent_type = SparseShape<FieldType>;

    /// Type of the PIMPL this class is derived from
    using base_type = ShapePIMPL<FieldType>;

    /// Type of this class
    using my_type = SparseShapePIMPL<FieldType>;

public:
    /// Type used to specify the lengths of each mode
    using typename base_type::extents_type;

    /// Type of the sparse map
    using sparse_map_type = typename parent_type::sparse_map_type;

    /// Type of a pointer to the base class
    using typename base_type::pimpl_pointer;

    /// Type of the class TA uses to describe tile sparsity
    using ta_shape_type = TA::SparseShape<float>;

    /// Type of the class TA uses to describe tiling
    using ta_tile_range = TA::TiledRange;

    /// Type of the object which maps indpendent/dependent indices to tensor
    /// modes
    using idx2mode_type = typename parent_type::idx2mode_type;

    /** @brief Creates a new SparseShapePIMPL with the provided state
     *
     *
     *  @param[in] extents The length of each mode.
     *
     *  @param[in] sm The SparseMap containing a map from independent indices to
     *                non-zero dependent indices.
     *
     *  @param[in] idx2mode Assuming @p sm contains independent indices of rank
     *                      @f$i@f$, and dependent indices of rank @f$d@f$,
     *                      @p idx2mode is an @f$i + d@f$ element vector such
     *                      that for @f$j<i@f$ `idx2mode[j]` is the mode of the
     *                      shape that the @f$j@f$-th independent mode maps to.
     *                      For @f$i <= j < i + d@f$ `idx2mode[j]` is the mode
     *                      of the shape that the @f$j-i@f$-th dependent mode
     *                      maps to.
     *
     *
     *  @throw std::runtime_error If the overall rank of @p sm is inconsistent
     *                            with the length of @p idx2mode. Strong throw
     *                            gurantee.
     *  @throw std::out_of_range If any element of @p idx2mode is larger than
     *                           the rank of the shape. Strong throw guarantee.
     */
    SparseShapePIMPL(extents_type x, sparse_map_type sm, idx2mode_type i2m);

    /** @brief Makes a deep copy of the provided SparseShapePIMPL instance.
     *
     *  This is a non-polymorphic copy of @p other meaning that if @p other was
     *  not instantiated as a SparseShapePIMPL instance, but rather as an
     *  instance of a class derived from SparseShapePIMPL, the copy will slice
     *  @p other.
     *
     *  This ctor is public so that the SparseShape class can call it from its
     *  non-polymorphic copy ctor.
     *
     *  @param[in] other the PIMPL being copied.
     *
     *  @throw std::bad_alloc if there is a problem copying @p other. Strong
     *                        throw guarantee.
     */
    SparseShapePIMPL(const SparseShapePIMPL& other) = default;

    /** @brief Computes the tile sparsity.
     *
     *  Given a tensor's tiling, this function uses the internal state of the
     *  shape to figure out which tiles are non-zero. The behavior differs
     *  slightly depending on the field type.
     *
     *  For `field::Scalar` the input tiling is assumed to be the tiling for the
     *  entire tensor (*i.e.*, the rank of @p tr should be equal to the rank of
     *  the tensor).
     *
     *  In ToTs only the outter indices (*i.e.* independent indices) are tiled,
     *  thus for `field::Tensor`, the rank of @p tr should equal the rank of the
     *  independent indices.
     *
     *
     *  @param[in] tr The tiling of the tensor, such that `tr[i]` is how the
     *                `i`-th mode is tiled.
     *
     *  @return An object which such that the `i`-th element is 0.0 if the
     *          `i`-th tile is zero, and `numeric_limits<float>::max()`
     *          otherwise.
     *
     *  @throw std::runtime_error if the rank of @p tr is not consistent with
     *                            the internal sparse map. Strong throw
     *                            guarantee.
     */
    ta_shape_type shape(const ta_tile_range& tr) const;

    /** @brief Non-polymorphic equality comparison.
     *
     *  This function will compare the SparseShapePIMPL part of the current
     *  instance to the SparseShapePIMPL part of @p rhs. The SparseShapePIMPL
     *  parts are equal if they contain the same sparse maps, map the sparse
     *  map to the tensor in the same way, and if the base class parts compare
     *  equal.
     *
     *  @param[in] rhs The instance we are comparing to.
     *
     *  @return True if this instance's SparseShapePIMPL part is equivalent to
     *          the SparseShapePIMPL part of @p rhs, false otherwise.
     *
     */
    bool operator==(const SparseShapePIMPL& rhs) const noexcept;

    const auto& sparse_map() const noexcept { return m_sm_; }

    const auto& idx2mode_map() const noexcept { return m_i2m_; }

protected:
    /// Additionally hashes the sparse map and index mapping
    void hash_(pluginplay::Hasher& h) const override;

private:
    /// Makes a polymorphic deep copy of this PIMPL
    virtual pimpl_pointer clone_() const override;

    /// The SparseMap object describing which elements are zero
    sparse_map_type m_sm_;

    /// The object used to convert from independent/dependent indices to modes
    /// of the tensor
    idx2mode_type m_i2m_;
};

extern template class SparseShapePIMPL<field::Scalar>;
extern template class SparseShapePIMPL<field::Tensor>;

} // namespace chemist::tensor::detail_
