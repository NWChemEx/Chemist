#pragma once
#include <memory>

namespace chemist::detail_ {

/** @brief Code factorization for implementing a view of an object
 *
 *  Views allow us to alias the state of an object and "view" it as if it had a
 *  specific API. The view encapsulates the conversion logic. In LibChemist we
 *  rely on the PIMPL idiom to decouple the API from the state/implementation. A
 *  natural way to encapsulate the conversion logic is to use different PIMPLs
 *  for owning vs. aliasing. Say we have a class `A` that is implemented in
 *  terms of an abstract base class `Base` and that we have two PIMPLs `Own` and
 *  `Alias`, which derive from `Base` such that `Own` implements `A` using
 *  memory it owns and `Alias` implements `A` using memory it aliases. Since `A`
 *  ultimately stores the `Own` or `Alias` instance via the common base class
 *  `Base`, the user has no way of knowing whether `A` owns or aliases its
 *  memory. This can lead to nasty side effects where an aliased object is
 *  accidentally modified when the user intents to only modify the `A` instance.
 *
 *  The ViewBase class represents code factorization for implementing a view by
 *  wrapping an aliasing instance. The use of separate class (as opposed to say
 *  templating `A` above on `Own` vs `Alias`) allows us to easily change the
 *  behavior of the view vs. the object. In particular the object `A` should
 *  have value semantics regardless of whether it aliases or owns its state
 *  (i.e. copying `A` should *always* result in a deep copy). On the other hand
 *  views are akin to other aliasing types such as iterators, pointers, or
 *  references so copying should be shallow. While it is possible to do this
 *  with template specialization and/or meta-template programming, maintaining
 *  two class hierarchies is ultimately easier to maintain.
 *
 *  Usage: Derive from this class and call `pimpl()` to get the instance that
 *         contains the aliased data. This class will automatically take care of
 *         const-correctness for your dervied class, so just use the result of
 *         the `pimpl()` function as is.
 *
 *  As a design note, the instance of `A` that aliases the data is owned by
 *  this class so as to facilitate shallow copying `ViewBase` instances without
 *  needing to know more about the API of `A` (in particular this allows `A` to
 *  use its copy ctor for deep copies).
 *
 * @tparam is_const In order to ensure we get the const-correctness of the view
 *                  right we need to know if the view should be read-only. The
 *                  @p is_const template non-type parameter should be set to
 *                  `true` if the resulting view is read-only and `false`
 *                  otherwise.
 *
 * @tparam AliasedType The class `A` in the description above. It is the type of
 *                     the class that holds the aliasing PIMPL. We ultimately
 *                     assume that @p AliasedType is moveable and can be
 *                     compared via `operator==`. @p AliasedType should be an
 *                     unqualified type.
 */
template<bool is_const, typename AliasedType>
class ViewBase {
private:
    /// The type of this instance
    using my_type = ViewBase<is_const, AliasedType>;
    /// Type of a pointer to the implementation with the aliased state
    using alias_ptr = std::shared_ptr<AliasedType>;
    /// Type of a read-only reference to the aliased object
    using const_alias = const AliasedType&;
    /// Type of a (possibly) read-/write-able reference to the aliased object
    using alias_reference =
      std::conditional_t<is_const, const_alias, AliasedType&>;
    /// Type of this class with the const-ness still unspecified
    template<bool rhs_const>
    using other_base = ViewBase<rhs_const, AliasedType>;

public:
    /// Type of the instance holding the aliased state, for meta-programming.
    using aliased_type = AliasedType;

    /** @brief Creates a new ViewBase that wraps the provided aliasing instance.
     *
     *  This is the primary ctor for this class. It takes the instance which
     *  contains the aliased state. This instance's lifetime is ultimately
     *  coupled to this view and all copies of this view.
     *
     *  @param[in] pimpl The instance aliasing the state. Since the instance
     *                   lives in this class you most likely will want to move
     *                   the instance into this class.
     *
     *  @throw std::bad_alloc if making the shared_ptr fails. Strong throw
     *         guarantee.
     *
     *  Complexity: constant
     */
    explicit ViewBase(AliasedType pimpl);

    /** @brief Makes a new ViewBase that aliases the same state as @p rhs.
     *
     *  ViewBase instances ultimately have pointer semantics, thus this ctor
     *  will create a new ViewBase instance, which contains a shared_ptr to
     *  the same implementing instance as @p rhs.
     *
     * @param[in] rhs The instance containing the state we are shallow copying.
     *
     * @throw none No throw guarantee.
     *
     * complexity: constant
     */
    ViewBase(const my_type& rhs) noexcept = default;

    /** @brief Makes the current ViewBase alias the same state as @p rhs.
     *
     *  This function will make the current ViewBase instance alias the same
     *  state as @p rhs. The state that the current instance was aliasing
     *  before this operation will be freed if the reference count goes to zero.
     *
     *  @param[in] rhs The ViewBase whose instance should be aliased.
     *
     *  @throw none No throw guarantee.
     *
     *  @return The current instance after making it alias @p rhs's state.
     */
    ViewBase& operator=(const my_type& rhs) noexcept = default;

    /** @brief Creates a new ViewBase which takes over ownership of @p rhs's
     *         state.
     *
     *  This ctor can be used to create a new ViewBase instance which aliases
     *  the same state as @p rhs. Unlike the copy ctor, after this ctor is
     *  called @p rhs will no longer alias the state.
     *
     * @param[in, out] rhs The instance whose aliased state is being taken.
     *                     After this function @p rhs will not alias any state.
     *
     * @throw none No throw guarantee.
     *
     * Complexity constant.
     */
    ViewBase(my_type&& rhs) noexcept = default;

    /** @brief Replaces the current instance's aliased state with that of
     *         @p rhs.
     *
     *  This function will cause the current instance to use the same aliasing
     *  instance as @p rhs. After this operation the state originally aliased by
     *  this instance have its reference count decreased by 1. Similarly @p rhs
     *  will no longer alias any state.
     *
     * @param[in, out] rhs The instance whose aliased state is being transferred
     *                     to this instance. After this function @p rhs will no
     *                     longer alias any state.
     *
     * @return The current instance, but now containing @p rhs's aliased state.
     *
     * @throw None no throw guarantee.
     *
     * Complexity: constant.
     */
    ViewBase& operator=(my_type&& rhs) noexcept = default;

    /// Default polymorphic dtor
    virtual ~ViewBase() noexcept = default;

    /** @brief Enables implicit conversion to a read-only instance of the
     *         aliased type.
     *
     *  If a user writes a function that needs to read data from a class, they
     *  likely do not want to overload it for a view of that class too. This
     *  conversion allows our view to be used as a read-only instance of that
     *  class. Since the resulting reference is read-only the aliasing nature of
     *  this instance is irrelevant and can be safely ignored by the function.
     *  This conversion also comes in to play when the user wants to break the
     *  aliasing. There it can be used to trigger the copy ctor of the wrapped
     *  instance.
     *
     *  @return A read-only reference to the instance storing the aliased state.
     *
     *  @throw None No throw guarantee.
     *
     *  Complexity: Constant.
     */
    operator const_alias() const noexcept { return *m_pimpl_; }

    /** @brief Allows comparing views to the underlying instance.
     *
     *  This comparison operator will compare the state aliased by the wrapped
     *  instance to the state of @p rhs. The actual comparison relies on
     *  `AliasedType::operator==` and therefore adopts its definition of
     *  equality.
     *
     *  @param[in] rhs The instance to compare to.
     *
     *  @return True if the wrapped instance compares equal to @p rhs and false
     *          otherwise.
     *
     *  @throw ??? If the wrapped instance's operator== throws. Same throw
     *             guarantee.
     *
     *  Complexity: Same as AliasedType::operator==
     */
    bool operator==(const_alias rhs) const;

    /** @brief Allows comparing to other views
     *
     *  This comparison operator will compare the state aliased by the wrapped
     *  instance to the state wrapped by @p rhs. The actual comparison relies on
     *  `AliasedType::operator==` and therefore adopts its definition of
     *  equality.
     *
     *  @tparam rhs_const Whether or not @p rhs is a read-only view.
     *
     *  @param[in] rhs The instance to compare to.
     *
     *  @return True if the wrapped instance compares equal to the instance
     *               wrapped by @p rhs and false otherwise.
     *
     *  @throw ??? If the wrapped instance's operator== throws. Same throw
     *             guarantee.
     *
     *  Complexity: Same as AliasedType::operator==
     */
    template<bool rhs_const>
    bool operator==(const other_base<rhs_const>& rhs) const;

    /** @brief Allows comparing views to the underlying instance.
     *
     *  This comparison operator will compare the state aliased by the wrapped
     *  instance to the state of @p rhs. The actual comparison relies on
     *  `AliasedType::operator==` and therefore adopts its definition of
     *  equality. This function negates the result of the equality comparison.
     *
     *  @param[in] rhs The instance to compare to.
     *
     *  @return False if the wrapped instance compares equal to @p rhs and true
     *          otherwise.
     *
     *  @throw ??? If the wrapped instance's operator== throws. Same throw
     *             guarantee.
     *
     *  Complexity: Same as AliasedType::operator==
     */
    bool operator!=(const_alias rhs) const;

    /** @brief Allows comparing to other views
     *
     *  This comparison operator will compare the state aliased by the wrapped
     *  instance to the state wrapped by @p rhs. The actual comparison relies on
     *  `AliasedType::operator==` and therefore adopts its definition of
     *  equality. This function simply negates the result of the equality
     *  comparison.
     *
     *  @tparam rhs_const Whether or not @p rhs is a read-only view.
     *
     *  @param[in] rhs The instance to compare to.
     *
     *  @return False if the wrapped instance compares equal to the instance
     *               wrapped by @p rhs and truee otherwise.
     *
     *  @throw ??? If the wrapped instance's operator== throws. Same throw
     *             guarantee.
     *
     *  Complexity: Same as AliasedType::operator==
     */
    template<bool rhs_const>
    bool operator!=(const other_base<rhs_const>& rhs) const;

protected:
    /** @brief Returns a (possibly) read-/write-able reference to the wrapped
     *         instance.
     *
     *  This function is meant to be used by the derived class to get the
     *  instance which holds the aliased data. Basic Template meta-programming
     *  will automatically take care of the const-ness of the return for you. In
     *  particular if this view is read-only (i.e. @p is_const is true) then you
     *  will get back a read-only reference, even though this is not the const
     *  version of `pimpl()`.
     *
     *  @return A (possibly) read-/write-able reference to the wrapped instance.
     *
     *  @throw None No throw guarantee.
     *
     *  Complexity: Constant.
     */
    alias_reference pimpl() noexcept { return *m_pimpl_; }

    /** @brief Returns a read-only reference to the wrapped instance.
     *
     *  This function is meant to be used by the derived class to get the
     *  instance which holds the aliased data. The returned instance is always
     *  read-only.
     *
     *  @return A read-only reference to the wrapped instance.
     *
     *  @throw None No throw guarantee.
     *
     *  Complexity: Constant.
     */
    const_alias pimpl() const noexcept { return *m_pimpl_; }

private:
    /// The instance actually holding the aliased data
    alias_ptr m_pimpl_;
}; // class ViewBase

// --------------------------Implementations-----------------------------------
#define VIEW_BASE_TEMPLATE_PARAMS template<bool is_const, typename AliasedType>
#define VIEW_BASE ViewBase<is_const, AliasedType>

VIEW_BASE_TEMPLATE_PARAMS
bool operator==(const AliasedType& lhs, const VIEW_BASE& rhs) {
    return lhs == static_cast<const AliasedType&>(rhs);
}

VIEW_BASE_TEMPLATE_PARAMS
bool operator!=(const AliasedType& lhs, const VIEW_BASE& rhs) {
    return !(lhs == static_cast<const AliasedType&>(rhs));
}

VIEW_BASE_TEMPLATE_PARAMS
VIEW_BASE::ViewBase(AliasedType pimpl) :
  m_pimpl_(std::make_shared<AliasedType>(std::move(pimpl))) {}

VIEW_BASE_TEMPLATE_PARAMS
bool VIEW_BASE::operator==(const_alias rhs) const { return pimpl() == rhs; }

VIEW_BASE_TEMPLATE_PARAMS
template<bool rhs_const>
bool VIEW_BASE::operator==(const other_base<rhs_const>& rhs) const {
    return pimpl() == static_cast<const AliasedType&>(rhs);
}

VIEW_BASE_TEMPLATE_PARAMS
bool VIEW_BASE::operator!=(const_alias rhs) const { return !((*this) == rhs); }

VIEW_BASE_TEMPLATE_PARAMS
template<bool rhs_const>
bool VIEW_BASE::operator!=(const other_base<rhs_const>& rhs) const {
    return !((*this) == rhs);
}

#undef VIEW_BASE
#undef VIEW_BASE_TEMPLATE_PARAMS
} // namespace chemist::detail_
