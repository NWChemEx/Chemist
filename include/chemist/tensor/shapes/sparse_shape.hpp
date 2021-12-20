#pragma once
#include "chemist/sparse_map/sparse_map.hpp"
#include "chemist/tensor/shapes/shape.hpp"

namespace chemist::tensor {

/** @brief Describes a tensor's shape when there's sparsity.
 *
 *  This class uses a SparseMap and an allocator to properly determine which
 *  tiles are 0.
 *
 *  @tparam FieldType Type of the elements in the field associated with the
 *                    tensor.
 */
template<typename FieldType>
class SparseShape : public Shape<FieldType> {
private:
    /// Type of the base class
    using base_type = Shape<FieldType>;

    /// Type of this class
    using my_type = SparseShape<FieldType>;

    /// False if T is the same as FieldType
    template<typename T>
    static constexpr bool different_fields_v = !std::is_same_v<T, FieldType>;

    /// Disables a function if T is the same as FieldType
    template<typename T>
    using disable_if_same_field_t = std::enable_if_t<different_fields_v<T>>;

public:
    /// Type returned by make_tensor
    using typename base_type::tensor_type;

    /// Type of an allocator for this tensor type
    using typename base_type::const_allocator_reference;

    /// Type used to describe the length of each mode
    using typename base_type::extents_type;

    /// Type when this class is returned as a pointer to the base class
    using typename base_type::pointer_type;

    /// Type of the sparse map taken as input
    using sparse_map_type = sparse_map::SparseMapEE;

    /// Type of a read-only reference to the sparse map the shape is using
    using const_sparse_map_reference = const sparse_map_type&;

    /// Type of the input mapping sparse map indices to tensor modes
    using idx2mode_type = std::vector<std::size_t>;

    /// Type of a read-only reference to the sparse map to tensor mode map
    using const_idx2mode_reference = const idx2mode_type&;

    /** @brief Creates a new SparseShape from extents and a SparseMap.
     *
     *  This ctor delegates to the primary constructor. It assumes that the
     *  mapping from an independent and dependent index to a tensor index is
     *  simply concatenation, *e.g.*, given independent index (1, 2) and
     *  dependent index (3, 4) this function assumes that the corresponding
     *  tensor index is (1, 2; 3, 4) (semi-colon separating outer and inner
     *  tensor indices). This constructor is primarily of use when the shape is
     *  describing a ToT.
     *
     *  @param[in] extents The length of each mode.
     *
     *  @param[in] sm The SparseMap specifying which dependent indices must be
     *                considered for a given independent index.
     *
     *  @throw std::runtime_error If the overall rank @p sm is inconsistent with
     *                            the length of @p extents. Strong throw
     *                            guarantee.
     */
    SparseShape(extents_type extents, sparse_map_type sm);

    /** @brief Creates a new SparseShape from extents, a SparseMap, and a
     *         mapping from SparseMap indices to tensor modes.
     *
     *  This is the primary constructor for creating a SparseShape.
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
    SparseShape(extents_type extents, sparse_map_type sm, idx2mode_type i2m);

    /** @brief Non-polymorphic comparison of two SparseShape instances.
     *
     *  Two SparseShape instances are equivalent if there base classes compare
     *  equal, they both have the same sparse map, and the sparse map indices
     *  are mapped to the tensor modes the same way. If a class derives from
     *  SparseShape, any state in the derived class is not considered by this
     *  comparison.
     *
     *  @param[in] rhs The instance we are comparing to.
     *
     *  @return True if this instance compares equivalent to @p rhs and false
     *          otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator==(const SparseShape& rhs) const noexcept;

    /** @brief Implements operator== for different fields.
     *
     *  If two SparseShape instances have different fields they are not equal.
     *  This function is chosen when the two SparseShape instances have
     *  different fields and will return false.
     *
     *  @tparam RHSField The type of field @p rhs is defined over. Assumed to be
     *                   either field::Scalar or field::Tensor.
     *  @tparam <anonymous> Used to enable this function, via SFINAE, when
     *                      @p RHSField is different from FieldType.
     *
     *  @param[in] rhs The instance we are comparing to.
     *
     *  @return False. This function is only used when the fields are different,
     *          in which case the shapes are also different.
     *
     *  @throw None No throw guarantee.
     */
    template<typename RHSField, typename = disable_if_same_field_t<RHSField>>
    bool operator==(const SparseShape<RHSField>& /*rhs*/) const noexcept {
        return false;
    }

    const_sparse_map_reference sparse_map() const;

    const_idx2mode_reference idx2mode_map() const;

protected:
    /** @brief Makes a non-polymorphic copy of this instance.
     *
     *  This copy operation will deep-copy all state in the SparseShape part
     * of the object (including the Shape base class). This function is
     * primarily used to implement Clone.
     *
     *  @param[in] other The instance we are copying.
     *
     *  @throw std::bad_alloc if there is a problem allocating memory. Strong
     *                        throw guarantee.
     */
    SparseShape(const SparseShape& other);

private:
    /// Overrides to account for SparseShape's state
    virtual pointer_type clone_() const override;

    /// Overrides make_tensor to account for sparsity
    virtual tensor_type make_tensor_(
      const_allocator_reference p) const override;

    virtual bool is_equal_(const Shape<FieldType>& rhs) const noexcept override;
};

/** @brief Determines if two SparseShape instances are different,
 *         non-polymorphically
 *
 *  @relates SparseShape
 *
 *  This function simply negates SparseShape::operator==. See documentation for
 *  SparseShape::operator== for what constitutes equivalency for SparseShape
 *  instances.
 *
 *  @tparam LHSField The type of the field associated with @p lhs. Assumed to be
 *                   either field::Scalar or field::Tensor.
 *  @tparam RHSField The type of the field associated with @p rhs. Assumed to be
 *                   either field::Scalar or field::Tensor.
 *
 *  @param[in] lhs The shape instance on the left side of the comparison.
 *  @param[in] rhs The shape instance on the right side of the comparison.
 *
 *  @return False if @p lhs and @p rhs compare equal and true otherwise.
 *
 *  @throw None No throw guarantee.
 */
template<typename LHSField, typename RHSField>
bool operator!=(const SparseShape<LHSField>& lhs,
                const SparseShape<RHSField>& rhs) {
    return !(lhs == rhs);
}

extern template class SparseShape<field::Scalar>;
extern template class SparseShape<field::Tensor>;

} // namespace chemist::tensor
