#include "libchemist/basis_set/shell/detail_/shell_pimpl.hpp"
#include "libchemist/basis_set/shell/shell.hpp"
#include "libchemist/point/detail_/point_pimpl.hpp"

namespace libchemist {

template<typename T>
Shell<T>::Shell() :
  m_pimpl_(std::make_unique<pimpl_t>()),
  Point<T>(),
  utilities::IndexableContainerBase<Shell<T>>() {}

template<typename T>
Shell<T>::Shell(const Shell<T>& rhs) :
  Shell(std::make_unique<pimpl_t>(*rhs.m_pimpl_),
        std::make_unique<point_pimpl_t>(rhs.x(), rhs.y(), rhs.z())) {}

template<typename T>
Shell<T>::Shell(Shell<T>&& rhs) noexcept = default;

template<typename T>
Shell<T>& Shell<T>::operator=(const Shell<T>& rhs) {
    return *this = std::move(Shell<T>(rhs));
}

template<typename T>
Shell<T>& Shell<T>::operator=(Shell<T>&& rhs) noexcept = default;

template<typename T>
Shell<T>::Shell(bool pure, int l, std::vector<T> coefs, std::vector<T> exps,
                T x, T y, T z) :
  Shell(std::make_unique<pimpl_t>(pure, l, std::move(coefs), std::move(exps)),
        std::make_unique<point_pimpl_t>(x, y, z)) {}

template<typename T>
Shell<T>::Shell(pimpl_ptr_t my_pimpl, point_pimpl_ptr_t base_pimpl) :
  m_pimpl_(std::move(my_pimpl)),
  Point<T>(std::move(base_pimpl)),
  utilities::IndexableContainerBase<Shell<T>>() {}

template<typename T>
Shell<T>::~Shell<T>() noexcept = default;

template<typename T>
bool& Shell<T>::pure() noexcept {
    return m_pimpl_->purity();
}

template<typename T>
const bool& Shell<T>::pure() const noexcept {
    return m_pimpl_->purity();
}

template<typename T>
typename Shell<T>::size_type& Shell<T>::l() noexcept {
    return m_pimpl_->l();
}

template<typename T>
const typename Shell<T>::size_type& Shell<T>::l() const noexcept {
    return m_pimpl_->l();
}

template<typename T>
typename Shell<T>::size_type Shell<T>::size_() const noexcept {
    return m_pimpl_->size();
}

template<typename T>
typename Shell<T>::reference Shell<T>::at_(size_type index) {
    auto ptr1 = m_pimpl_->at(index);
    auto ptr2 = this->point_alias();
    ContractedGaussian<T> temp(std::move(ptr1), std::move(ptr2));
    return reference(std::move(temp));
}

template<typename T>
typename Shell<T>::const_reference Shell<T>::at_(size_type index) const {
    auto ptr1 = m_pimpl_->at(index);
    auto ptr2 = this->point_alias();
    ContractedGaussian<T> temp(std::move(ptr1), std::move(ptr2));
    return const_reference(std::move(temp));
}

template class Shell<double>;
template class Shell<float>;

} // namespace libchemist
