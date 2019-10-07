#pragma once
#include "libchemist/basis_set/contracted_gaussian/contracted_gaussian.hpp"

namespace libchemist {

/** @brief Models a reference to a ContractedGaussian
 *
 *  This class models a reference to a ContractedGaussian. Instances of this
 *  class are aliases and any modifications made through the API will modify
 *  the aliased instance.
 *
 *  @tparam T The cv-qualified type of the parameters stored in the aliased
 *          ContractedGaussian.
 */
template<typename T>
class ContractedGaussianView
  : public utilities::IndexableContainerBase<ContractedGaussianView<T>> {
private:
    /// Type of this instance
    using my_type = ContractedGaussianView<T>;
    /// Type of the base class
    using base_type = utilities::IndexableContainerBase<my_type>;
    /// Type of the template type parameter with cv-qualifiers removed
    using no_cv_t = std::remove_cv_t<T>;
    /// True if this an alias of a read-only container
    static constexpr bool is_const = std::is_const_v<T>;

public:
    /// The type of the elements in this container
    using value_type = Primitive<no_cv_t>;
    /// Type used for indexing/offsets is unsigned, integral POD type
    using size_type = std::size_t;

    /** @brief Creates a ContractedGaussianView that does not alias a
     *         ContractedGaussian.
     *
     *  This ctor is used to create a new ContractedGaussianView instance which
     *  does not alias a ContractedGaussianView instance. The resulting instance
     *  is primarily intended as a placeholder and attempting to access the
     *  members of the instance is undefined behavior. The resulting instance
     *  can be made into a valid instance by copy/move assignment.
     *
     *  @throw std::bad_alloc if there is insufficient memory to create the
     *         PIMPL. Strong throw guarantee.
     */
    ContractedGaussianView();

    ContractedGaussianView(std::vector<T*> coefs, std::vector<T*> exps, T* x,
                           T* y, T* z);
    operator const ContractedGaussian<no_cv_t>&() const { return m_pimpl_; }

private:
    /// Allows IndexableContainerBase to implement this class
    friend base_type;
    /// Implements `size()` for IndexableContainerBase
    size_type size_() const noexcept { return m_pimpl_.size(); }
    /// Implements `operator[]` for IndexableContainerBase
    decltype(auto) at_(size_type i);
    /// Implements `operator[]cosnt` for IndexableContainerBase
    decltype(auto) at_(size_type i) const { return m_pimpl_[i]; }

    /// The instance that actually implements this class
    ContractedGaussian<no_cv_t> m_pimpl_;
}; // End class contracted_gaussian

// ---------------------------------Implementations-----------------------------

template<typename T>
decltype(auto) ContractedGaussianView<T>::at_(size_type i) {
    if constexpr(is_const) {
        const my_type& const_this = *this;
        return const_this.at_(i);
    } else {
        return m_pimpl_[i];
    }
}

extern template class ContractedGaussianView<double>;
extern template class ContractedGaussianView<const double>;
extern template class ContractedGaussianView<float>;
extern template class ContractedGaussianView<const float>;

} // namespace libchemist
