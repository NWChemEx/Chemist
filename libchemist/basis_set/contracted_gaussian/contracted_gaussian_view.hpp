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

    ContractedGaussianView(ContractedGaussian<no_cv_t> pimpl);

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

/** @brief Overloads ContractedGaussian<T>::operator== for views.
 *
 *  @relates ContractedGaussian
 *
 *  All equality comparisons involving ContractedGaussianView instances simply
 *  cast all views to read-only ContractedGaussian instances and then defer to
 *  ContractedGaussian's operator==.
 *
 * @tparam T The template type of the instance on the left side of the operator.
 * @tparam U The template type of the instance on the right side of the operator
 * @param[in] lhs The CGTO on the left of the operator.
 * @param[in] rhs The CGTO on the right of the operator.
 *
 * @return True if @p lhs aliases a CGTO that has the same state as @p rhs
 *         and false otherwise.
 *
 * @throw none No throw guarantee.
 */
template<typename T, typename U>
bool operator==(const ContractedGaussianView<T>& lhs,
                const ContractedGaussian<U>& rhs) noexcept {
    using value_type = std::remove_cv_t<T>;
    const ContractedGaussian<value_type>& cast_lhs(lhs);
    return cast_lhs == rhs;
}

/** @brief Overloads ContractedGaussian<T>::operator== for views.
 *
 *  @relates ContractedGaussian
 *
 *  All equality comparisons involving ContractedGaussianView instances simply
 *  cast all views to read-only ContractedGaussian instances and then defer to
 *  ContractedGaussian's operator==.
 *
 * @tparam T The template type of the instance on the left side of the operator.
 * @tparam U The template type of the instance on the right side of the operator
 * @param[in] lhs The CGTO on the left of the operator.
 * @param[in] rhs The CGTO on the right of the operator.
 *
 * @return True if CGTO @p lhs has the same state as the CGTO that @p rhs
 *         aliases and false otherwise.
 *
 * @throw none No throw guarantee.
 */
template<typename T, typename U>
bool operator==(const ContractedGaussian<T>& lhs,
                const ContractedGaussianView<U>& rhs) noexcept {
    using value_type = std::remove_cv_t<U>;
    const ContractedGaussian<value_type>& cast_rhs(rhs);
    return lhs == cast_rhs;
}

/** @brief Overloads ContractedGaussian<T>::operator== for views.
 *
 *  @relates ContractedGaussian
 *
 *  All equality comparisons involving ContractedGaussianView instances simply
 *  cast all views to read-only ContractedGaussian instances and then defer to
 *  ContractedGaussian's operator==.
 *
 * @tparam T The template type of the instance on the left side of the operator.
 * @tparam U The template type of the instance on the right side of the operator
 * @param[in] lhs The CGTO on the left of the operator.
 * @param[in] rhs The CGTO on the right of the operator.
 *
 * @return True if the CGTO aliased by @p lhs has the same state as the CGTO
 *         that @p rhs aliases and false otherwise.
 *
 * @throw none No throw guarantee.
 */
template<typename T, typename U>
bool operator==(const ContractedGaussianView<T>& lhs,
                const ContractedGaussianView<U>& rhs) noexcept {
    using lhs_type = std::remove_cv_t<T>;
    using rhs_type = std::remove_cv_t<U>;
    const ContractedGaussian<lhs_type>& cast_lhs(lhs);
    const ContractedGaussian<rhs_type>& cast_rhs(rhs);
    return cast_lhs == cast_rhs;
}

/** @brief Overloads ContractedGaussian<T>::operator!= for views.
 *
 *  @relates ContractedGaussian
 *
 *  All inequality comparisons for ContractedGaussianView<T> simply negate the
 *  result of the equality comparison.
 *
 * @tparam T The template type of the instance on the left side of the operator.
 * @tparam U The template type of the instance on the right side of the operator
 * @param[in] lhs The CGTO on the left of the operator.
 * @param[in] rhs The CGTO on the right of the operator.
 *
 * @return Falsee if @p lhs aliases a CGTO that has the same state as @p rhs
 *         and true otherwise.
 *
 * @throw none No throw guarantee.
 */
template<typename T, typename U>
bool operator!=(const ContractedGaussianView<T>& lhs,
                const ContractedGaussian<U>& rhs) noexcept {
    return !(lhs == rhs);
}

/** @brief Overloads ContractedGaussian<T>::operator!= for views.
 *
 *  @relates ContractedGaussian
 *
 *  All inequality comparisons for ContractedGaussianView<T> simply negate the
 *  result of the equality comparison.
 *
 * @tparam T The template type of the instance on the left side of the operator.
 * @tparam U The template type of the instance on the right side of the operator
 * @param[in] lhs The CGTO on the left of the operator.
 * @param[in] rhs The CGTO on the right of the operator.
 *
 * @return False if the CGTO, @p lhs, has the same state as the CGTO aliased by
 *         @p rhs and true otherwise.
 *
 * @throw none No throw guarantee.
 */
template<typename T, typename U>
bool operator!=(const ContractedGaussian<T>& lhs,
                const ContractedGaussianView<U>& rhs) noexcept {
    return !(lhs == rhs);
}

/** @brief Overloads ContractedGaussian<T>::operator!= for views.
 *
 *  @relates ContractedGaussian
 *
 *  All inequality comparisons for ContractedGaussianView<T> simply negate the
 *  result of the equality comparison.
 *
 * @tparam T The template type of the instance on the left side of the operator.
 * @tparam U The template type of the instance on the right side of the operator
 * @param[in] lhs The CGTO on the left of the operator.
 * @param[in] rhs The CGTO on the right of the operator.
 *
 * @return False if the CGTO aliased by @p lhs has the same state as the CGTO
 *         aliased by @p rhs and true otherwise.
 *
 * @throw none No throw guarantee.
 */
template<typename T, typename U>
bool operator!=(const ContractedGaussianView<T>& lhs,
                const ContractedGaussianView<U>& rhs) noexcept {
    return !(lhs == rhs);
}

// ---------------------------------Implementations-----------------------------
template<typename T>
ContractedGaussianView<T>::ContractedGaussianView(
  ContractedGaussian<no_cv_t> pimpl) :
  m_pimpl_(std::move(pimpl)) {}

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
