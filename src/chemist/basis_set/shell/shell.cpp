#include "../../point/point_pimpl.hpp"
#include "chemist/basis_set/shell.hpp"
#include "shell_pimpl.hpp"
#include <cassert>

namespace chemist {

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
Shell<T>::Shell(ShellType pure, int l, std::vector<T> coefs,
                std::vector<T> exps, T x, T y, T z) :
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
typename Shell<T>::pure_type& Shell<T>::pure() noexcept {
    assert(m_pimpl_ != nullptr);
    return m_pimpl_->purity();
}

template<typename T>
const typename Shell<T>::pure_type& Shell<T>::pure() const noexcept {
    assert(m_pimpl_ != nullptr);
    return m_pimpl_->purity();
}

template<typename T>
typename Shell<T>::size_type& Shell<T>::l() noexcept {
    assert(m_pimpl_ != nullptr);
    return m_pimpl_->l();
}

template<typename T>
const typename Shell<T>::size_type& Shell<T>::l() const noexcept {
    assert(m_pimpl_ != nullptr);
    return m_pimpl_->l();
}

template<typename T>
typename Shell<T>::size_type Shell<T>::n_unique_primitives() const noexcept {
    return at_(0).size();
}

template<typename T>
typename Shell<T>::primitive_reference Shell<T>::unique_primitive(size_type i) {
    return (*this)[0][i];
}

template<typename T>
typename Shell<T>::const_primitive_reference Shell<T>::unique_primitive(
  size_type i) const {
    return (*this)[0][i];
}

template<typename T>
typename Shell<T>::size_type Shell<T>::size_() const noexcept {
    assert(m_pimpl_ != nullptr);
    return m_pimpl_->size();
}

template<typename T>
typename Shell<T>::reference Shell<T>::at_(size_type index) {
    assert(m_pimpl_ != nullptr);
    auto ptr1 = m_pimpl_->at(index);
    auto ptr2 = this->point_alias();
    ContractedGaussian<T> temp(std::move(ptr1), std::move(ptr2));
    return reference(std::move(temp));
}

template<typename T>
typename Shell<T>::const_reference Shell<T>::at_(size_type index) const {
    assert(m_pimpl_ != nullptr);
    auto ptr1 = m_pimpl_->at(index);
    auto ptr2 = this->point_alias();
    ContractedGaussian<T> temp(std::move(ptr1), std::move(ptr2));
    return const_reference(std::move(temp));
}

template class Shell<double>;
template class Shell<float>;

} // namespace chemist
