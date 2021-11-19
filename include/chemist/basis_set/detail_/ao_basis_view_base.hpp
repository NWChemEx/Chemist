#pragma once
#include "chemist/point/point_view.hpp"
#include <utilities/containers/indexable_container_base.hpp>

namespace chemist::detail_ {

/** @brief Code factorization for the various view classes associated with the
 *         AOBasisSet class.
 *
 *  The various classes associated with the AOBasisSet all follow the same
 *  pattern. There is a PIMPL class, call it P, that implements the API
 *  class A (API classes being Primitive, ContractedGaussian, etc.). We then
 *  need to implement a view of A, which is done by a third class, call it V. In
 *  all cases the way we make the view is:
 *
 *  1. We create an instance of P that aliases the state
 *  2. We stick the aliasing P instance into an A instance (the resulting A
 *     instance still aliases the state)
 *  3. We stick the aliasing A instance into a V instance, thereby making it
 *     clear to the user, via the type, that they have an alias of an A
 *     instance.
 *
 *  As a result of this pattern most of the views are implemented in the same
 *  manner. The ViewBase class factors the common implementations out into a
 *  single class. Each class still needs to forward its specific member
 *  functions (for example ShellView needs to forward the accessor for
 *  determining if the shell is pure and the total angular momentum of the
 *  shell).
 *
 * @tparam T The cv-qualified type used to model the parameters in ViewType
 * @tparam AliasedType A template template parameter giving the class we are
 *                     aliasing (`A` in the above description).
 *
 * @tparam ViewType The type of the class we are implementing
 */
template<typename T, template<typename> class AliasedType, typename ViewType>
class AOBasisViewBase : public PointView<T, AliasedType<std::remove_cv_t<T>>>,
                        public utilities::IndexableContainerBase<ViewType> {
private:
    /// Type of this instance
    using my_type = ViewType;
    /// Type of the base class
    using container_base_type = utilities::IndexableContainerBase<my_type>;
    /// Type of the template type parameter with cv-qualifiers removed
    using no_cv_t = std::remove_cv_t<T>;
    /// Type of the PointViewBase
    using point_base_type = PointView<T, AliasedType<no_cv_t>>;
    /// Type of another view with (possibly) different cv qualifications
    template<typename U>
    using other_ao_basis_view = AOBasisViewBase<U, AliasedType, ViewType>;

public:
    /// The type aliased by this class
    using aliased_type = AliasedType<no_cv_t>;
    /// The type of the elements in this container
    using value_type = typename aliased_type::value_type;
    /// Type used for indexing/offsets is an unsigned, integral POD type
    using size_type = typename container_base_type::size_type;

    AOBasisViewBase(aliased_type) noexcept;
    using PointView<T, aliased_type>::operator==;
    using PointView<T, aliased_type>::operator!=;

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
    template<typename U>
    bool operator==(const other_ao_basis_view<U>& rhs) const noexcept;

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
    template<typename U>
    bool operator!=(const other_ao_basis_view<U>& rhs) const noexcept;

private:
    /// Allows IndexableContainerBase to access the hooks
    friend container_base_type;
    /// Implements size()
    size_type size_() const noexcept { return this->pimpl().size(); }
    /// Implements operator[]
    decltype(auto) at_(size_type i) { return this->pimpl()[i]; }
    /// Implements operator[]()const
    decltype(auto) at_(size_type i) const { return this->pimpl()[i]; }
}; // class ViewBase

// ------------------------ Implementations -----------------------------------
/// Cleans up the declaration of all the template parameters for ViewBase class
#define VIEW_BASE_TEMPLATE                                     \
    template<typename T, template<typename> class AliasedType, \
             typename ViewType>
/// Cleans up the ViewBase type when it has all the parameters specified
#define VIEW_BASE_TYPE AOBasisViewBase<T, AliasedType, ViewType>

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
VIEW_BASE_TEMPLATE
bool operator==(const AliasedType<std::remove_cv_t<T>>& lhs,
                const VIEW_BASE_TYPE& rhs) noexcept {
    return lhs == static_cast<const AliasedType<std::remove_cv_t<T>>&>(rhs);
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
VIEW_BASE_TEMPLATE
bool operator!=(const AliasedType<std::remove_cv_t<T>>& lhs,
                const VIEW_BASE_TYPE& rhs) noexcept {
    return !(lhs == rhs);
}

VIEW_BASE_TEMPLATE
VIEW_BASE_TYPE::AOBasisViewBase(aliased_type pimpl) noexcept :
  PointView<T, aliased_type>(std::move(pimpl)),
  utilities::IndexableContainerBase<ViewType>() {}

VIEW_BASE_TEMPLATE
template<typename U>
bool VIEW_BASE_TYPE::operator==(
  const other_ao_basis_view<U>& rhs) const noexcept {
    const aliased_type& alias = *this;
    return rhs == alias;
}

VIEW_BASE_TEMPLATE
template<typename U>
bool VIEW_BASE_TYPE::operator!=(
  const other_ao_basis_view<U>& rhs) const noexcept {
    return !(*this == rhs);
}

#undef VIEW_BASE_TYPE
#undef VIEW_BASE_TEMPLATE

} // namespace chemist::detail_
