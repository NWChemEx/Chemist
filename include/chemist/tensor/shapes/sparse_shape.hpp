#pragma once
#include "chemist/sparse_map/sparse_map.hpp"
#include "chemist/tensor/shapes/shape.hpp"

namespace chemist::tensor {

template<typename FieldType>
class SparseShape : public Shape<FieldType> {
private:
    using base_type = Shape<FieldType>;

    using my_type = SparseShape<FieldType>;

public:
    using typename base_type::tensor_type;

    using typename base_type::const_allocator_reference;

    using typename base_type::extents_type;

    using typename base_type::pointer_type;

    using sparse_map_type = sparse_map::SparseMapEE;

    SparseShape(extents_type extents, sparse_map_type sm);

protected:
    SparseShape(const SparseShape& other);

private:
    virtual pointer_type clone_() const override;

    virtual tensor_type make_tensor_(
      const_allocator_reference p) const override;
};

extern template class SparseShape<field::Scalar>;
extern template class SparseShape<field::Tensor>;

} // namespace chemist::tensor
