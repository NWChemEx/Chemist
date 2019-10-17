#include "libchemist/basis_set/contracted_gaussian/contracted_gaussian.hpp"
#include "libchemist/basis_set/contracted_gaussian/detail_/cgto_pimpl.hpp"
#include "libchemist/point/detail_/point_pimpl.hpp"

namespace libchemist {

template<typename T>
ContractedGaussian<T>::ContractedGaussian() :
  m_pimpl_(std::make_unique<pimpl_type>()),
  Point<T>(),
  utilities::IndexableContainerBase<ContractedGaussian>() {}

template<typename T>
ContractedGaussian<T>::ContractedGaussian(std::vector<T> coefs,
                                          std::vector<T> exps, T x, T y, T z) :
  m_pimpl_(std::make_unique<pimpl_type>(
    utilities::MathSet<T>(coefs.begin(), coefs.end()),
    utilities::MathSet<T>(exps.begin(), exps.end()))),
  Point<T>(x, y, z),
  utilities::IndexableContainerBase<ContractedGaussian>() {}

template<typename T>
ContractedGaussian<T>::ContractedGaussian(
  pimpl_ptr my_pimpl, point_pimpl_ptr point_pimpl) noexcept :
  m_pimpl_(std::move(my_pimpl)),
  Point<T>(std::move(point_pimpl)),
  utilities::IndexableContainerBase<ContractedGaussian>() {}

template<typename T>
ContractedGaussian<T>::ContractedGaussian(const my_type& rhs) :
  ContractedGaussian(
    [&]() {
        std::vector<T> coefs(rhs.size());
        for(size_type i = 0; i < rhs.size(); ++i)
            coefs[i] = rhs[i].coefficient();
        return coefs;
    }(),
    [&]() {
        std::vector<T> exps(rhs.size());
        for(size_type i = 0; i < rhs.size(); ++i) exps[i] = rhs[i].exponent();
        return exps;
    }(),
    rhs.x(), rhs.y(), rhs.z()) {}

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
typename ContractedGaussian<T>::size_type ContractedGaussian<T>::size_() const
  noexcept {
    return m_pimpl_->size();
}

template<typename T>
typename ContractedGaussian<T>::reference ContractedGaussian<T>::at_(
  size_type i) {
    return {&(m_pimpl_->coef(i)), &(m_pimpl_->exp(i)), &(this->x()),
            &(this->y()), &(this->z())};
}

template<typename T>
typename ContractedGaussian<T>::const_reference ContractedGaussian<T>::at_(
  size_type i) const {
    return {&(m_pimpl_->coef(i)), &(m_pimpl_->exp(i)), &(this->x()),
            &(this->y()), &(this->z())};
}

template class ContractedGaussian<double>;
template class ContractedGaussian<float>;

} // namespace libchemist
