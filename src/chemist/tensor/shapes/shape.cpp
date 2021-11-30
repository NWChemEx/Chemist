#include "detail_/shape_pimpl.hpp"

namespace chemist::tensor {
namespace {

template<typename FieldType, typename... Args>
auto make_pimpl(Args&&... args) {
    using return_type = detail_::ShapePIMPL<FieldType>;
    return std::make_unique<return_type>(std::forward<Args>(args)...);
}

} // namespace

#define SHAPE Shape<FieldType>

//------------------------------------------------------------------------------
//                            CTors, DTor, and Assignment
//------------------------------------------------------------------------------

template<typename FieldType>
SHAPE::Shape() noexcept = default;

template<typename FieldType>
SHAPE::Shape(extents_type extents) :
  m_pimpl_(make_pimpl<FieldType>(std::move(extents))) {}

template<typename FieldType>
SHAPE::Shape(const Shape& other) :
  m_pimpl_(other.has_pimpl_() ? make_pimpl<FieldType>(other.pimpl_()) :
                                nullptr) {}

template<typename FieldType>
SHAPE::~Shape() noexcept = default;

//------------------------------------------------------------------------------
//                               Accessors
//------------------------------------------------------------------------------

template<typename FieldType>
typename SHAPE::extents_type SHAPE::extents() const {
    if(m_pimpl_) return m_pimpl_->extents();
    return extents_type{};
}

//------------------------------------------------------------------------------
//                      Protected/Private Functions
//------------------------------------------------------------------------------

template<typename FieldType>
const typename SHAPE::pimpl_type& SHAPE::pimpl_() const {
    assert_pimpl_();
    return *m_pimpl_;
}

template<typename FieldType>
bool SHAPE::has_pimpl_() const noexcept {
    return m_pimpl_.operator bool();
}

template<typename FieldType>
void SHAPE::assert_pimpl_() const {
    if(has_pimpl_()) return;
    throw std::runtime_error("Instance does not contain a PIMPL. Did you "
                             "default construct it or move from it?");
}

template<typename FieldType>
typename SHAPE::pointer_type SHAPE::clone_() const {
    return pointer_type(new Shape(*this));
}

template<typename FieldType>
typename SHAPE::tensor_type SHAPE::make_tensor_(
  const_allocator_reference p) const {
    assert_pimpl_();
    auto tr     = p.make_tiled_range(m_pimpl_->extents());
    auto& world = p.runtime();
    return tensor_type(std::in_place_index<0>, world, tr);
}

#undef SHAPE

template class Shape<field::Scalar>;
template class Shape<field::Tensor>;

} // namespace chemist::tensor
