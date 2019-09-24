#pragma once
#include "libchemist/basis_set/contracted_gaussian/contracted_gaussian.hpp"

namespace libchemist {

/** @brief Models a reference to a Gaussian formed by contracting primitives
 *
 *  @tparam T The cv-qualified type of the parameters stored in the aliased
 *          ContractedGaussian.
 */
template<typename T>
class ContractedGaussianView {
private:
    using my_type = ContractedGaussianView<T>;

public:
    using value_type      = std::remove_cv_t<T>;
    using reference       = PrimitiveView<T>;
    using const_reference = PrimitiveView<const T>;
    using size_type       = std::size_t;
    using iterator        = utilities::iterators::OffsetIterator<my_type>;
    using const_iterator  = utilities::iterators::OffsetIterator<const my_type>;

    ContractedGaussianView();
    ContractedGaussianView(std::vector<T*> coefs, std::vector<T*> exps, T* x,
                           T* y, T* z);
    ~ContractedGaussianView() noexcept;

    size_type size() const noexcept { return m_pimpl_.size(); }
    reference operator[](size_type i) { return m_pimpl_[i]; }
    const_reference operator[](size_type i) const { return m_pimpl_[i]; }

    iterator begin() noexcept { return m_pimpl_.begin(); }
    const_iterator begin() const noexcept { return m_pimpl_.begin(); }
    const_iterator cbegin() const noexcept { return m_pimpl_.cbegin(); }

    iterator end() noexcept { return m_pimpl_.end(); }
    const_iterator end() const noexcept { return m_pimpl_.end(); }
    const_iterator cend() const noexcept { return m_pimpl_.cend(); }

private:
    ContractedGaussian<value_type> m_pimpl_;
}; // End class contracted_gaussian

extern template class ContractedGaussianView<double>;
extern template class ContractedGaussianView<const double>;
extern template class ContractedGaussianView<float>;
extern template class ContractedGaussianView<const float>;

} // namespace libchemist
