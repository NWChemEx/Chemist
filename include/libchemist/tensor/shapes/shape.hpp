// Will be re-enabled in a future PR
// #pragma once
// #include <vector>

// namespace libchemist::tensor::shape {

// template<typename FieldType>
// class Shape {
// private:
//     using my_type = Shape<FieldType>;

// public:
//     using field_type     = FieldType;
//     using allocator_type = Allocator<field_type>;
//     using extents_type   = typename allocator_type::extents_type;
//     using ta_shape_type  = TA::SparseShape<float>;
//     using shape_ptr      = std::unique_ptr<my_type>;

//     ta_shape_type make_shape(const allocator_type& p);

// private:
//     virtual ta_shape_type make_shape_(const allocator_type& p) const;

//     extents_type m_extents_;
// };

// template<typename FieldType>
// typename Shape<FieldType>::ta_shape_type Shape<FieldType>::make_shape_(
//   const allocator_type& p) const {
//     return ta_shape_type(1, p.make_tiled_range(m_extents_));
// }

// } // namespace libchemist::tensor::shape
