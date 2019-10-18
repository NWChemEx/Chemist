#pragma once
#include <utilities/containers/indexable_container_base.hpp>

namespace libchemist::detail_ {

/** @brief Code factorization for the various view classes associated with the
 *         AOBasisSet class.
 *
 *  The various classes associated with the AOBasisSet all follow the same
 *  pattern. There is a PIMPL class, call it P, that implements the API
 *  class A (API classes being Primitive, ContractedGaussian, etc.). We then
 *  need to implement a view of A, which is done by a class V. In all cases this
 *  works the same way:
 *
 *  1. We create an instance of P that aliases the state
 *  2. We stick the P instance into an A instance (which in turn still aliases
 *     the state)
 *  3. We stick the A instance into a V instance, thereby making it clear to the
 *     user, via the type, that they have an alias of an A instance.
 *
 *  As a result of this pattern most of the views are implemented in the same
 *  manner. The ViewBase class factors the common implementations out into a
 *  single class.
 *
 * @tparam T The cv-qualified type used to model the parameters in ViewType
 * @tparam AliasedType A template template parameter giving the class we are
 *                     aliasing (`A` in the above description).
 *
 * @tparam ViewType The type of the class we are implementing
 */
template<typename T, template<typename> class AliasedType, typename ViewType>
class ViewBase : public utilities::IndexableContainerBase<ViewType> {
private:
    /// Type of this instance
    using my_type = ViewType;
    /// Type of the base class
    using base_type = utilities::IndexableContainerBase<my_type>;
    /// Type of the template type parameter with cv-qualifiers removed
    using no_cv_t = std::remove_cv_t<T>;
    /// True if this an alias of a read-only container
    static constexpr bool is_const = std::is_const_v<T>;

public:
    /// The type aliased by this class
    using aliased_type = AliasedType<no_cv_t>;
    /// The type of the elements in this container
    using value_type = typename aliased_type::value_type;
    /// Type used for indexing/offsets is an unsigned, integral POD type
    using size_type = typename base_type::size_type;

    /** @brief Constructs a view around the provided instance.
     *
     *  The value provided to this ctor is the instance of `A` mentioned in
     *  this class's description, namely, an instance of the class this view
     *  aliases, such that @p pimpl's PIMPL aliases the state.
     *
     *  @param[in] pimpl The instance we are  aliasing
     *
     *  @throw none No throw guarantee.
     */
    explicit ViewBase(aliased_type pimpl) : m_pimpl_(std::move(pimpl)) {}

    /** @brief Allows the view to be used as a read-only instance of the
     *         wrapped class.
     *
     *  This conversion operator allows the view to be implicitly converted to a
     *  read-only instance of the class the view is aliasing. This is useful so
     *  that end-users do not need to worry about templating to handle the view
     *  and value instances.
     *
     *  @return The instance used to implement the view in a read-only manner.
     *
     *  @throw none No throw guarantee.
     */
    operator const aliased_type&() const noexcept { return m_pimpl_; }

    /** @brief Allows the view to be compared to an instance of the aliased type
     *
     *  This function ultimately works by explicitly calling the conversion
     *  operator and relying on the aliased type's operator== to perform the
     *  comparison.
     *
     *  @param[in] rhs The instance to compare to.
     *
     *  @return True if the aliased type's operator== returns true and false
     *          otherwise.
     *  @throw none No throw guarantee.
     */
    bool operator==(const aliased_type& rhs) const noexcept;

    /** @brief Allows the view to be compared to another view
     *
     *  This function ultimately works by explicitly calling the conversion
     *  operator and relying on the aliased type's operator== to perform the
     *  comparison.
     *
     *  @tparam U The cv-qualified type used to store parameters in @p rhs.
     *  @tparam V The type of derived type of @p rhs
     *
     *  @param[in] rhs The instance to compare to.
     *
     *  @return True if the aliased type's operator== returns true and false
     *          otherwise.
     *  @throw none No throw guarantee.
     */
    template<typename U, typename V>
    bool operator==(const ViewBase<U, AliasedType, V>& rhs) const noexcept;

    /** @brief Allows the view to be compared to an instance of the aliased type
     *
     *  This function ultimately works by negating the result of the aliased
     *  type's operator== member.
     *
     *  @param[in] rhs The instance to compare to.
     *
     *  @return False if the aliased type's operator== returns true and true
     *                otherwise.
     *
     *  @throw none No throw guarantee.
     */
    bool operator!=(const aliased_type& rhs) const noexcept;

    /** @brief Allows two views to be compared
     *
     *  This function ultimately works by negating the result of calling the
     *  analogous operator== member.
     *
     *  @tparam U the cv-qualified type used to store parameters in @p rhs.
     *  @tparam V the type of the class derived from @p rhs.
     *
     *  @param[in] rhs The instance to compare to.
     *
     *  @return False if operator== returns true and true otherwise.
     *
     *  @throw none No throw guarantee.
     */
    template<typename U, typename V>
    bool operator!=(const ViewBase<U, AliasedType, V>& rhs) const noexcept;

protected:
    /// Const-correct type of the PIMPL
    using pimpl_type =
      std::conditional_t<is_const, const aliased_type&, aliased_type&>;

    /** @brief Allows derived classes to get a (possibly) read-/write- version
     *         of the PIMPL.
     *
     *  This function returns the PIMPL taking into account const-correctness,
     *  namely if T is const-qualified you will get back a read-only reference,
     *  whereas if T is non-const you get a read-/write- reference.
     *
     *  @return The wrapped instance in a read-/write- state if @p T is
     *          non-const and a read-only version if @p T is const.
     *
     *  @throw none No throw guarantee.
     */
    pimpl_type pimpl() noexcept { return m_pimpl_; }
    /// Allows derived classes to get a read-only version of the PIMPL
    const aliased_type& pimpl() const noexcept { return m_pimpl_; }

private:
    /// Allows IndexableContainerBase to access the hooks
    friend base_type;
    /// Implements size()
    size_type size_() const noexcept { return pimpl().size(); }
    /// Implements operator[]
    decltype(auto) at_(size_type i) { return pimpl()[i]; }
    /// Implements operator[]()const
    decltype(auto) at_(size_type i) const { return pimpl()[i]; }
    /// The instance actually containing the aliased state
    aliased_type m_pimpl_;
}; // class ViewBase

/** @brief Allows the type aliased by a view to be on the left side of a
 *         comparison with a view.
 *
 *  @relates ViewBase
 *
 *  This function ensures that `aliased_type == view` works. This function
 *  ultimately works by calling `rhs == lhs`.
 *
 *  @tparam T The cv-qualified type used for the parameters in the view
 *  @tparam AliasedType The class the view implements an alias of
 *  @tparam ViewType The derived class ViewBase is implementing
 *
 *  @param[in] lhs The instance on the left-side of the operator.
 *  @param[in] rhs The instance on the right-side of the operator.
 *
 *  @return True if the aliased type's operator== returns true and false
 *          otherwise.
 *
 *  @throw none No throw guarantee.
 */
template<typename T, template<typename> class AliasedType, typename ViewType>
bool operator==(const AliasedType<std::remove_cv_t<T>>& lhs,
                const ViewBase<T, AliasedType, ViewType>& rhs) noexcept {
    return rhs == lhs;
}

/** @brief Allows the type aliased by a view to be on the left side of a
 *         comparison with a view.
 *
 *  @relates ViewBase
 *
 *  This function ensures that `aliased_type != view` works. This function
 *  ultimately works by negating `lhs == rhs`.
 *
 *  @tparam T The cv-qualified type used for the parameters in the view
 *  @tparam AliasedType The class the view implements an alias of
 *  @tparam ViewType The derived class ViewBase is implementing
 *
 *  @param[in] lhs The instance on the left-side of the operator.
 *  @param[in] rhs The instance on the right-side of the operator.
 *
 *  @return False if the aliased type's operator== returns true and true
 *          otherwise.
 *
 *  @throw none No throw guarantee.
 */
template<typename T, template<typename> class AliasedType, typename ViewType>
bool operator!=(const AliasedType<std::remove_cv_t<T>>& lhs,
                const ViewBase<T, AliasedType, ViewType>& rhs) noexcept {
    return !(lhs == rhs);
}

// ------------------------ Implementations -----------------------------------
/// Cleans up the declaration of all the template parameters for ViewBase class
#define VIEW_BASE_TEMPLATE                                     \
    template<typename T, template<typename> class AliasedType, \
             typename ViewType>
/// Cleans up the ViewBase type when it has all the parameters specified
#define VIEW_BASE_TYPE ViewBase<T, AliasedType, ViewType>

VIEW_BASE_TEMPLATE
bool VIEW_BASE_TYPE::operator==(const aliased_type& rhs) const noexcept {
    const aliased_type& alias = *this;
    return alias == rhs;
}

VIEW_BASE_TEMPLATE
template<typename U, typename V>
bool VIEW_BASE_TYPE::operator==(const ViewBase<U, AliasedType, V>& rhs) const
  noexcept {
    const aliased_type& alias = *this;
    return rhs == alias;
}

VIEW_BASE_TEMPLATE
bool VIEW_BASE_TYPE::operator!=(const aliased_type& rhs) const noexcept {
    return !(*this == rhs);
}

VIEW_BASE_TEMPLATE
template<typename U, typename V>
bool VIEW_BASE_TYPE::operator!=(const ViewBase<U, AliasedType, V>& rhs) const
  noexcept {
    return !(*this == rhs);
}

#undef VIEW_BASE_TYPE
#undef VIEW_BASE_TEMPLATE

} // namespace libchemist::detail_
