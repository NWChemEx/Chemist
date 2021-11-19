#include "../../point/point_pimpl.hpp"
#include "cgto_pimpl.hpp"
#include "chemist/basis_set/contracted_gaussian.hpp"
#include <cassert>

namespace chemist {

template<typename T>
ContractedGaussian<T>::ContractedGaussian() :
  m_pimpl_(std::make_unique<pimpl_type>()),
  Point<T>(),
  utilities::IndexableContainerBase<ContractedGaussian>() {}

template<typename T>
ContractedGaussian<T>::ContractedGaussian(std::vector<T> coefs,
                                          std::vector<T> exps, T x, T y, T z) :
  m_pimpl_(std::make_unique<pimpl_type>(std::move(coefs), std::move(exps))),
  Point<T>(x, y, z),
  utilities::IndexableContainerBase<ContractedGaussian>() {}

template<typename T>
ContractedGaussian<T>::ContractedGaussian(pimpl_ptr my_pimpl,
                                          point_pimpl_ptr point_pimpl) noexcept
  :
  m_pimpl_(std::move(my_pimpl)),
  Point<T>(std::move(point_pimpl)),
  utilities::IndexableContainerBase<ContractedGaussian>() {}

template<typename T>
ContractedGaussian<T>::ContractedGaussian(const my_type& rhs) :
  m_pimpl_(std::make_unique<pimpl_type>(*rhs.m_pimpl_)),
  Point<T>(rhs),
  utilities::IndexableContainerBase<ContractedGaussian>() {}

template<typename T>
ContractedGaussian<T>::ContractedGaussian(my_type&& rhs) noexcept = default;

template<typename T>
ContractedGaussian<T>& ContractedGaussian<T>::operator=(const my_type& rhs) {
    return *this = std::move(my_type(rhs));
}

template<typename T>
ContractedGaussian<T>& ContractedGaussian<T>::operator=(
  my_type&& rhs) noexcept = default;

template<typename T>
ContractedGaussian<T>::~ContractedGaussian() noexcept = default;

template<typename T>
typename ContractedGaussian<T>::size_type ContractedGaussian<T>::size_()
  const noexcept {
    assert(m_pimpl_ != nullptr);
    return m_pimpl_->size();
}

template<typename T>
typename ContractedGaussian<T>::reference ContractedGaussian<T>::at_(
  size_type i) {
    assert(m_pimpl_ != nullptr);
    return PrimitiveView<T>(
      Primitive<T>(std::move(m_pimpl_->at(i)), std::move(this->point_alias())));
}

template<typename T>
typename ContractedGaussian<T>::const_reference ContractedGaussian<T>::at_(
  size_type i) const {
    assert(m_pimpl_ != nullptr);
    return PrimitiveView<const T>(
      Primitive<T>(std::move(m_pimpl_->at(i)), std::move(this->point_alias())));
}

template class ContractedGaussian<double>;
template class ContractedGaussian<float>;

} // namespace chemist
