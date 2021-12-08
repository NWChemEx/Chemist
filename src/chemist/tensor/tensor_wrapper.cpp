#include "chemist/tensor/detail_/pimpl.hpp"

namespace chemist::tensor {

// Macro to avoid typing the full type of the TensorWrapper
#define TENSOR_WRAPPER TensorWrapper<FieldType>

//------------------------------------------------------------------------------
//                            Ctors
//------------------------------------------------------------------------------

template<typename FieldType>
TENSOR_WRAPPER::TensorWrapper() = default;

template<typename FieldType>
TENSOR_WRAPPER::TensorWrapper(allocator_pointer p) :
  TensorWrapper(variant_type{}, std::move(p)) {}

template<typename FieldType>
TENSOR_WRAPPER::TensorWrapper(const shape_type& shape, allocator_pointer p) :
  TensorWrapper(p->new_tensor(shape), std::move(p)) {}

template<typename FieldType>
template<typename OtherField, typename>
TENSOR_WRAPPER::TensorWrapper(const TensorWrapper<OtherField>& other,
                              shape_pointer pshape, allocator_pointer palloc) {}

template<typename FieldType>
TENSOR_WRAPPER::TensorWrapper(variant_type v, allocator_pointer p) :
  m_pimpl_(std::make_unique<pimpl_type>(std::move(v), std::move(p))) {}

template<typename FieldType>
TENSOR_WRAPPER::TensorWrapper(const TensorWrapper& other) :
  m_pimpl_(other.m_pimpl_ ? other.pimpl_().clone() : nullptr) {}

template<typename FieldType>
TENSOR_WRAPPER::TensorWrapper(TensorWrapper&& other) = default;

template<typename FieldType>
TENSOR_WRAPPER& TENSOR_WRAPPER::operator=(const TensorWrapper& other) {
    if(this == &other) return *this;

    if(other.m_pimpl_)
        other.pimpl_().clone().swap(m_pimpl_);
    else
        m_pimpl_.reset();

    return *this;
}

template<typename FieldType>
TENSOR_WRAPPER& TENSOR_WRAPPER::operator=(TensorWrapper&& other) = default;

template<typename FieldType>
TENSOR_WRAPPER::~TensorWrapper() noexcept = default;

//------------------------------------------------------------------------------
//                           Accessors
//------------------------------------------------------------------------------

template<typename FieldType>
typename TENSOR_WRAPPER::const_allocator_reference TENSOR_WRAPPER::allocator()
  const {
    return pimpl_().allocator();
}

template<typename FieldType>
void TENSOR_WRAPPER::reallocate(allocator_pointer p) {
    pimpl_().reallocate(std::move(p));
}

template<typename FieldType>
typename TENSOR_WRAPPER::labeled_tensor_type TENSOR_WRAPPER::operator()(
  const annotation_type& annotation) {
    return labeled_tensor_type(annotation, *this);
}

template<typename FieldType>
typename TENSOR_WRAPPER::const_labeled_tensor_type TENSOR_WRAPPER::operator()(
  const annotation_type& annotation) const {
    return const_labeled_tensor_type(annotation, *this);
}

template<typename FieldType>
typename TENSOR_WRAPPER::annotation_type TENSOR_WRAPPER::make_annotation(
  const annotation_type& letter) const {
    return m_pimpl_ ? pimpl_().make_annotation(letter) : "";
}

template<typename FieldType>
typename TENSOR_WRAPPER::rank_type TENSOR_WRAPPER::rank() const {
    if(m_pimpl_) return pimpl_().rank();
    return rank_type{0};
}

template<typename FieldType>
typename TENSOR_WRAPPER::extents_type TENSOR_WRAPPER::extents() const {
    if(m_pimpl_) return pimpl_().extents();
    return extents_type{};
}

template<typename FieldType>
typename TENSOR_WRAPPER::size_type TENSOR_WRAPPER::size() const {
    if(m_pimpl_) return pimpl_().size();
    return size_type{0};
}

template<typename FieldType>
TENSOR_WRAPPER TENSOR_WRAPPER::slice(const il_type& lo, const il_type& hi,
                                     allocator_pointer p) const {
    TENSOR_WRAPPER rv;
    rv.m_pimpl_ = pimpl_().slice(lo, hi, std::move(p));
    return rv;
}

template<typename FieldType>
TENSOR_WRAPPER TENSOR_WRAPPER::reshape(shape_pointer shape) const {
    TENSOR_WRAPPER rv(*this);
    rv.pimpl_().reshape(std::move(shape));
    return rv;
}

template<typename FieldType>
std::ostream& TENSOR_WRAPPER::print(std::ostream& os) const {
    if(!m_pimpl_) return os;
    return pimpl_().print(os);
}

template<typename FieldType>
void TENSOR_WRAPPER::hash(pluginplay::Hasher& h) const {
    if(m_pimpl_) pimpl_().hash(h);
}

template<typename FieldType>
bool TENSOR_WRAPPER::operator==(const TensorWrapper& rhs) const {
    if(m_pimpl_ && rhs.m_pimpl_)
        return pimpl_() == rhs.pimpl_();
    else if(!m_pimpl_ && !rhs.m_pimpl_)
        return true;
    return false;
}

//------------------------------------------------------------------------------
//                  Protected and Private Members
//------------------------------------------------------------------------------

template<typename FieldType>
typename TENSOR_WRAPPER::variant_type& TENSOR_WRAPPER::variant_() {
    return pimpl_().variant();
}

template<typename FieldType>
const typename TENSOR_WRAPPER::variant_type& TENSOR_WRAPPER::variant_() const {
    return pimpl_().variant();
}

template<typename FieldType>
typename TENSOR_WRAPPER::labeled_variant_type TENSOR_WRAPPER::annotate_(
  const annotation_type& annotation) {
    return pimpl_().annotate(annotation);
}

template<typename FieldType>
typename TENSOR_WRAPPER::const_labeled_type TENSOR_WRAPPER::annotate_(
  const annotation_type& annotation) const {
    return pimpl_().annotate(annotation);
}

template<typename FieldType>
typename TENSOR_WRAPPER::pimpl_reference TENSOR_WRAPPER::pimpl_() {
    if(!m_pimpl_)
        m_pimpl_ = std::make_unique<pimpl_type>(
          variant_type{}, default_allocator<field_type>());
    return *m_pimpl_;
}

template<typename FieldType>
typename TENSOR_WRAPPER::const_pimpl_reference TENSOR_WRAPPER::pimpl_() const {
    if(m_pimpl_) return *m_pimpl_;
    throw std::runtime_error(
      "Tensor has no PIMPL. Was it default constructed or moved from?");
}

#undef TENSOR_WRAPPER

template TensorWrapper<field::Tensor>::TensorWrapper<field::Scalar, void>(
  const TensorWrapper<field::Scalar>&,
  typename TensorWrapper<field::Tensor>::shape_pointer,
  typename TensorWrapper<field::Tensor>::allocator_pointer);

template class TensorWrapper<field::Scalar>;
template class TensorWrapper<field::Tensor>;

} // namespace chemist::tensor
