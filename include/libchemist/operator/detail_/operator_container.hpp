#pragma once
#include "libchemist/operator/detail_/operator_base.hpp"
#include "libchemist/operator/type_traits/type_traits.hpp"
#include <memory>

namespace libchemist::operators::detail_ {
class OperatorContainerPIMPL;

/** @brief Code factorization for a container filled with operators
 *
 *  OperatorContainer stores a series of operators in a type-erased manner. This
 *  is accomplished by storing them as pointers to the OperatorBase instances.
 *  As a first pass we are going to make the OperatorContainer own the operators
 *  it is given. This means it will copy them in unless you move them in.
 *
 */
class OperatorContainer : public OperatorBase {
public:
    /// Pointer type used when referring to an operator through OperatorBase
    template<typename OpType>
    using operator_pointer = std::shared_ptr<OpType>;

    /// Read-only pointer to an operator's OperatorBase instance
    template<typename OpType>
    using const_operator_pointer = operator_pointer<const OpType>;

    /// The type used to return operators.
    template<typename OpType>
    using get_return_type = typename std::vector<operator_pointer<OpType>>;

    /// The type used to return read-only operators
    template<typename OpType>
    using get_const_return_type =
      typename std::vector<const_operator_pointer<OpType>>;

    /// Type of a pointer to a type-erased operator
    using type_erased_pointer = operator_pointer<OperatorBase>;

    /// Type of a vector of type-erased operators
    using type_erased_vector = get_return_type<OperatorBase>;

    /// Type of a read-only vector of type-erased operators
    using const_type_erased_vector = get_const_return_type<OperatorBase>;

    /// Type used to store RTTI
    using rtti_type = std::type_index;

    /// Defaulted no-throw dtor
    virtual ~OperatorContainer() noexcept;

    /** @brief Add an additonal operator to the OperatorContainer.
     *
     *  This function adds an operator to the OperatorContainer. The weight of
     *  the operation in the linear combination is part of the operator's state.
     *  If the OperatorContainer is in a PIMPL-less state, this function will
     *  generate a PIMPL instance.
     *
     *  @tparam OpType The type of the operator to add to the OperatorContainer.
     *                 Must be derived from OperatorBase.
     *
     *  @param[in] op The operator to add to this OperatorContainer.
     *
     *  @return A reference to the current OperatorContainer instance, after
     *          @p op has been added.
     *
     *  @throw std::bad_alloc if there is insufficient memory either to create
     *                        the PIMPL or to store the internal state of the
     *                        OperatorBase instance. Basic exception gurantee.
     *
     */
    template<typename OpType>
    OperatorContainer& add_term(OpType&& op);

    /** @brief Add one or more operators to the OperatorContainer.
     *
     *  This function adds one or more operators to the current
     *  OperatorContainer. If the OperatorContainer is in a PIMPL-less state,
     *  this function will generate a PIMPL instance.
     *
     *  @tparam Ops A parameter pack containing the types of the operators to
     *              add to the OperatorContainer. All types in @p Ops must be
     *              derived from OperatorBase.
     *
     *  @param[in] ops The operators to add to this OperatorContainer.
     *
     *  @return A reference to the current OperatorContainer instance after the
     *          operators have been added to it.
     *
     *  @throw std::bad_alloc if there is insufficient memory either to create
     *                        the PIMPL or to store any of @p ops. Basic
     *                        exception gurantee.
     */
    template<typename... Ops>
    OperatorContainer& add_terms(Ops&&... ops);

    /** @brief Returns the collection of operator terms matching a specific
     *         operator type contained in this OperatorContainer.
     *
     *  Obtains the unique collection of operator terms contained in the
     *  current OperatorContainer instance which match a specific type. If the
     *  Operator is not represented in this OperatorContainer or if
     *  the OperatorContainer is in a PIMPL-less state, an empty collection is
     *  returned.
     *
     *  @tparam OpType The type from which to query the OperatorContainer
     *
     *  @returns A vector of shared_ptr<OpType> which contains all of the
     *           operators in this OperatorContainer that match OpType.
     *
     *  @throw std::bad_alloc if there is insufficient memroy to allocate
     *                        the return value
     */
    template<typename OpType>
    get_return_type<OpType> get_terms();

    template<typename OpType>
    get_const_return_type<OpType> get_terms() const;

    /** @brief Return whether an Operator type is represented in this
     *         OperatorContainer.
     *
     *  Return a boolean which indicates whether an instance of OpType is
     *  contained in this OperatorContainer. If this OperatorContainer is
     *  in a PIMPL-less state, `false` is returned.
     *
     *  @tparam OpType The type we are looking for in this OperatorContainer
     *
     *  @returns `true` if `OpType` is represented in this OperatorContainer,
     *           `false` otherwise.
     *
     *  @throw None No throw gurantee
     *
     */
    template<typename OpType>
    bool has_term() const noexcept {
        return has_term_(typeid(OpType));
    }

    /** @brief Non-polymorphic equality comparison of OperatorContainer
     *         instances
     *
     *  Compare OperatorContainer instances without reference to their
     *  polymorphic instantiations - OperatorContainer instances which
     *  contain the same set of operators (with the same state) will
     *  be considered equal. PIMPL-less instances will also be
     conpluginplayred
     *  equal.
     *
     *  @param[in] other OperatorContainer instance we want to compare to
     *  @return `true` if this instance has an identical operator collection
     *           to @p other, `false` otherwise.
     */
    bool operator==(const OperatorContainer& other) const;

    /** Negation of `operator==`
     *
     *  @param[in] other OperatorContainer instance we want to compare to
     *  @return `false` if this instance has an identical operator collection
     *           to @p other, `true` otherwise.
     */
    bool operator!=(const OperatorContainer& other) const;

protected:
    /** @brief Creates a new OperatorContainer instance containing no operators.
     *
     *  This ctor can be used to create a new OperatorContainer instance which
     *  contains no operators. Operators may be added by calling `add_term`
     *  or `add_terms`.
     *
     *  @throw None No throw gurantee.
     *
     *  Complexity: Constant
     */
    OperatorContainer() noexcept;

    /** @brief Creates a new OperatorContainer instance by deep copying another
     *         instance.
     *
     *  This ctor can be used to create a new OperatorContainer instance which
     *  contains a deep copy of @p other
     *
     *  @param[in] other OperatorContainer instance to copy
     *
     *  @throw std::bad_alloc if there is insufficient memory to copy the
     *                        OperatorContainer instance. Strong exception
     *                        gurantee.
     *
     *  Complexity: Linear in the number of operators which comprise @p other.
     */
    OperatorContainer(const OperatorContainer& other);

    /** @brief Creates a new OperatorContainer instance by taking ownership of
     *         another instance's state.
     *
     *  This ctor will create a new OperatorContainer instance by taking
     *  ownership of @p other's state.
     *
     *  @param[in,out] other The instance whose state will be transferred to
     *                       the resulting instance. After the operation
     *                       @p other will not contain a PIMPL and will thus not
     *                       be usable until another PIMPL-containing
     *                       OperatorContainer instance is assigned to it.
     *
     *  @throw None No throw guarantee.
     *
     *  Complexity: Constant.
     */
    OperatorContainer(OperatorContainer&& other) noexcept;

    /** @brief Deep copy the state of another OperatorContainer instance to the
     *         current instance.
     *
     *  This function will deep copy the state of @p other to the current
     *  instance. The state previously held by this instance will be released.
     *
     *  @param[in] other The OperatorContainer instance to deep copy.
     *
     *  @return A reference to the current instance which contains a deep copy
     *          of @p other.
     *
     *  @throw std::bad_alloc if there is insufficient memory to copy the
     *                        OperatorContainer instance @p other. Strong
     *                        exception gurantee.
     *
     *  Complexity: Linear in the number of operators which comprise @p other.
     */
    OperatorContainer& operator=(const OperatorContainer& other);

    /** @brief Transfer the ownership of the state of another OperatorContainer
     *         instance to the current instance.
     *
     *  This function will transfer ownership of @p other's state to the
     *  current instance. The state previously held by this instance will
     *  be released.
     *
     *  @param[in,out] other The OperatorContainer instance whose state is to be
     *                       taken. @p other will no longer contain a valid
     *                       PIMPL and will need to be reassigned prior to
     *                       future usage.
     *
     *  @return The current instance after taking ownership of @p other's state.
     *
     *  @throw None No throw guarantee.
     *
     *  Complexity: Constant.
     */
    OperatorContainer& operator=(OperatorContainer&& other) noexcept;

    /** @brief Creates a new OperatorContainer instance from a collection of
     *         Operator instances.
     *
     *  This ctor can be used to create a OperatorContainer instance from
     *  one or more Operator instances. Additional terms may be
     *  added by invoking `add_term` or `add_terms`.
     *
     *  @tparam OpType Type of the first operator. Must be derived from
     *                 OperatorBase.
     *  @tparam Args Parameter pack of the remaining operators' types, must all
     *               be derived from OperatorBase.
     *
     *  @tparam <anonymous> Type used to disabled this overload, via SFINAE, if
     *                      @p OpType is not an operator. If any of @p Args are
     *                      not derived from OperatorBase a static assert will
     *                      be tripped in add_term.
     *  @param[in] op0 The first operator in the OperatorContainer.
     *  @param[in] args Operator instances from which to construct the
     *                  OperatorContainer.
     *
     *  @throw std::bad_alloc if there is insufficient memory either to create
     *                        the PIMPL or to store the internal state of any
     *                        Operator instance. Basic  exception gurantee.
     */
    template<typename OpType, typename... Args,
             typename = enable_if_operator_t<OpType>>
    explicit OperatorContainer(OpType&& op0, Args&&... args);

    /// Derived implementation of polymorphic equality comparison
    bool is_equal_impl(const OperatorBase& other) const noexcept override;

    void hash_impl(pluginplay::Hasher& h) const override;
    std::string as_string_impl() const override;

private:
    OperatorContainerPIMPL& pimpl_();
    const OperatorContainerPIMPL& pimpl_() const;

    /// Type erased private API for `add_term` which delegates to
    /// HamiltonanPIMPL::add_term
    void add_term_(rtti_type index, type_erased_pointer ptr);

    /// Type erased private API for `get_terms` which delegates to
    /// HamiltonanPIMPL::get_terms
    const_type_erased_vector get_terms_(rtti_type index) const;

    type_erased_vector get_terms_(rtti_type index);

    /// Type erased private API for `has_term` which delegates to
    /// HamiltonanPIMPL::has_term
    bool has_term_(rtti_type index) const noexcept;

    /// The instance actually implementing the API
    std::unique_ptr<OperatorContainerPIMPL> m_pimpl_;

}; // class OperatorContainer

// -----------------------------------------------------------------------------
// ------------------------ Inline Implementations -----------------------------
// -----------------------------------------------------------------------------

template<typename OpType>
OperatorContainer& OperatorContainer::add_term(OpType&& op) {
    using clean_t = std::decay_t<OpType>;
    static_assert(is_operator_v<clean_t>, "Argument must be an operator.");
    const auto& rtti = typeid(clean_t);
    add_term_(rtti, std::make_shared<clean_t>(std::forward<OpType>(op)));
    return *this;
}

template<typename... OpTypes>
OperatorContainer& OperatorContainer::add_terms(OpTypes&&... ops) {
    if constexpr(sizeof...(OpTypes) > 0) {
        (add_term(std::forward<OpTypes>(ops)), ...);
    }
    return *this;
}

template<typename OpType>
typename OperatorContainer::get_const_return_type<OpType>
OperatorContainer::get_terms() const {
    static_assert(is_operator_v<OpType>, "OpType must be an operator.");
    auto type_erased_terms = get_terms_(typeid(OpType));
    get_const_return_type<OpType> ret_terms(type_erased_terms.size());
    std::transform(type_erased_terms.begin(), type_erased_terms.end(),
                   ret_terms.begin(),
                   std::dynamic_pointer_cast<OpType, OperatorBase>);
    return ret_terms;
}

template<typename OpType>
typename OperatorContainer::get_return_type<OpType>
OperatorContainer::get_terms() {
    static_assert(is_operator_v<OpType>, "OpType must be an operator.");
    auto type_erased_terms = get_terms_(typeid(OpType));
    get_return_type<OpType> ret_terms(type_erased_terms.size());
    std::transform(type_erased_terms.begin(), type_erased_terms.end(),
                   ret_terms.begin(),
                   std::dynamic_pointer_cast<OpType, OperatorBase>);
    return ret_terms;
}

template<typename OpType, typename... Args, typename>
OperatorContainer::OperatorContainer(OpType&& op0, Args&&... args) :
  OperatorContainer() {
    add_terms(std::forward<OpType>(op0), std::forward<Args>(args)...);
}

} // namespace libchemist::operators::detail_
