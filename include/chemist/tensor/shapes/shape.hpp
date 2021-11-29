#pragma once
#include "chemist/tensor/fields.hpp"
#include <vector>

namespace chemist::tensor::shape {
namespace detail_ {
template<typename FieldType>
class ShapePIMPL;

extern template class ShapePIMPL<field::Scalar>;
extern template class ShapePIMPL<field::Tensor>;

} // namespace detail_

/** @brief Object describing a tensor's elemental layout.
 *
 *  The shape of a tensor is usually taken to mean the number of modes and
 *  the lengths of each of said modes. Here we generalize a bit and also
 *  include the sparsity as part of the shape.
 *
 *  @tparam FieldType The types of the elements in the tensor. Expected to
 *                    be either Scalar or Tensor.
 */
template<typename FieldType>
class Shape {
private:
    /// Type of this instance
    using my_type = Shape<FieldType>;

    ///

public:
    using field_type     = FieldType;
    using allocator_type = Allocator<field_type>;
    using extents_type   = typename allocator_type::extents_type;
    using ta_shape_type  = TA::SparseShape<float>;
    using shape_ptr      = std::unique_ptr<my_type>;

    ta_shape_type make_shape(const allocator_type& p);

private:
    virtual ta_shape_type make_shape_(const allocator_type& p) const;

    extents_type m_extents_;
};

template<typename FieldType>
typename Shape<FieldType>::ta_shape_type Shape<FieldType>::make_shape_(
  const allocator_type& p) const {
    return ta_shape_type(1, p.make_tiled_range(m_extents_));
}

} // namespace chemist::tensor::shape
