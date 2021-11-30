#include "detail_/sparse_shape_pimpl.hpp"

namespace chemist::tensor {

template<typename T>
using pimpl_type = detail_::SparseShapePIMPL<T>;

namespace {

template<typename FieldType>
const auto& downcast(const detail_::ShapePIMPL<FieldType>& pimpl) {
    using pimpl_t      = pimpl_type<FieldType>;
    const auto* ppimpl = dynamic_cast<const pimpl_t*>(&pimpl);
    if(ppimpl) return *ppimpl;

    // Cast failed (shouldn't be reachable unless I missed something)
    throw std::bad_cast("PIMPL is not a SparseShapePIMPL.");
}

template<typename FieldType, typename... Args>
auto make_pimpl(Args&&... args) {
    return std::make_unique<pimpl_type<FieldType>>(std::forward<Args>(args)...);
}

} // namespace

#define SPARSE_SHAPE SparseShape<FieldType>

template<typename FieldType>
SPARSE_SHAPE::SparseShape(extents_type extents, sparse_map_type sm) :
  base_type(make_pimpl<FieldType>(std::move(extents), std::move(sm))) {}

template<typename FieldType>
SPARSE_SHAPE::SparseShape(const SparseShape& other) :
  base_type(other.has_pimpl_() ?
              make_pimpl<FieldType>(downcast(other.pimpl_())) :
              typename base_type::pimpl_pointer{}) {}

//------------------------------------------------------------------------------
//                   Protected/Private Member Functions
//------------------------------------------------------------------------------

template<typename FieldType>
typename SPARSE_SHAPE::pointer_type SPARSE_SHAPE::clone_() const {
    return std::make_unique<my_type>(*this);
}

template<typename FieldType>
typename SPARSE_SHAPE::tensor_type SPARSE_SHAPE::make_tensor_(
  const_allocator_reference p) const {
    const auto& the_pimpl = downcast(this->pimpl_());
    auto tr               = p.make_tiled_range(the_pimpl.extents());
    auto& world           = p.runtime();
    // auto shape            = the_pimpl.shape();
    return tensor_type(std::in_place_index<0>, world, tr); //, shape);
}

#undef SPARSE_SHAPE

} // namespace chemist::tensor
