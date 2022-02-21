#pragma once
#include "chemist/operators/detail_/operator_base.hpp"
#include "chemist/operators/type_traits/type_traits.hpp"
#include <memory>

namespace chemist::operators::detail_ {
class LinearCombinationPIMPL;

/** @brief Code factorization for a container filled with operators
 *
 *  LinearCombination stores a series of operators in a type-erased manner. This
 *  is accomplished by storing them as pointers to the OperatorBase instances.
 *  As a first pass we are going to make the LinearCombination own the operators
 *  it is given. This means it will copy them in unless you move them in.
 *
 */
class LinearCombination : public OperatorBase {
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

    /// Type used for sizes, indexing, and offsets
    using size_type = std::size_t;

    /// Defaulted no-throw dtor
    virtual ~LinearCombination() noexcept;

    size_type size() const noexcept;

    /** @brief Add an additonal operator to the LinearCombination.
     *
     *  This function adds an operator to the LinearCombination. The weight of
     *  the operation in the linear combination is part of the operator's state.
     *  If the LinearCombination is in a PIMPL-less state, this function will
     *  generate a PIMPL instance.
     *
     *  @tparam OpType The type of the operator to add to the LinearCombination.
     *                 Must be derived from OperatorBase.
     *
     *  @param[in] op The operator to add to this LinearCombination.
     *
     *  @return A reference to the current LinearCombination instance, after
     *          @p op has been added.
     *
     *  @throw std::bad_alloc if there is insufficient memory either to create
     *                        the PIMPL or to store the internal state of the
     *                        OperatorBase instance. Basic exception gurantee.
     *
     */
    template<typename OpType>
    LinearCombination& add_term(OpType&& op);

    /** @brief Add one or more operators to the LinearCombination.
     *
     *  This function adds one or more operators to the current
     *  LinearCombination. If the LinearCombination is in a PIMPL-less state,
     *  this function will generate a PIMPL instance.
     *
     *  @tparam Ops A parameter pack containing the types of the operators to
     *              add to the LinearCombination. All types in @p Ops must be
     *              derived from OperatorBase.
     *
     *  @param[in] ops The operators to add to this LinearCombination.
     *
     *  @return A reference to the current LinearCombination instance after the
     *          operators have been added to it.
     *
     *  @throw std::bad_alloc if there is insufficient memory either to create
     *                        the PIMPL or to store any of @p ops. Basic
     *                        exception gurantee.
     */
    template<typename... Ops>
    LinearCombination& add_terms(Ops&&... ops);

    /** @brief Returns the collection of operator terms matching a specific
     *         operator type contained in this LinearCombination.
     *
     *  Obtains the unique collection of operator terms contained in the
     *  current LinearCombination instance which match a specific type. If the
     *  Operator is not represented in this LinearCombination or if
     *  the LinearCombination is in a PIMPL-less state, an empty collection is
     *  returned.
     *
     *  @tparam OpType The type from which to query the LinearCombination
     *
     *  @returns A vector of shared_ptr<OpType> which contains all of the
     *           operators in this LinearCombination that match OpType.
     *
     *  @throw std::bad_alloc if there is insufficient memroy to allocate
     *                        the return value
     */
    template<typename OpType>
    get_return_type<OpType> get_terms();

    template<typename OpType>
    get_const_return_type<OpType> get_terms() const;

    /** @brief Return whether an Operator type is represented in this
     *         LinearCombination.
     *
     *  Return a boolean which indicates whether an instance of OpType is
     *  contained in this LinearCombination. If this LinearCombination is
     *  in a PIMPL-less state, `false` is returned.
     *
     *  @tparam OpType The type we are looking for in this LinearCombination
     *
     *  @returns `true` if `OpType` is represented in this LinearCombination,
     *           `false` otherwise.
     *
     *  @throw None No throw gurantee
     *
     */
    template<typename OpType>
    bool has_term() const noexcept {
        return has_term_(typeid(OpType));
    }

    /** @brief Non-polymorphic equality comparison of LinearCombination
     *         instances
     *
     *  Compare LinearCombination instances without reference to their
     *  polymorphic instantiations - LinearCombination instances which
     *  contain the same set of operators (with the same state) will
     *  be considered equal. PIMPL-less instances will also be considered
     *  equal.
     *
     *  @param[in] other LinearCombination instance we want to compare to
     *  @return `true` if this instance has an identical operator collection
     *           to @p other, `false` otherwise.
     */
    bool operator==(const LinearCombination& other) const;

    /** Negation of `operator==`
     *
     *  @param[in] other LinearCombination instance we want to compare to
     *  @return `false` if this instance has an identical operator collection
     *           to @p other, `true` otherwise.
     */
    bool operator!=(const LinearCombination& other) const;

protected:
    /** @brief Creates a new LinearCombination instance containing no operators.
     *
     *  This ctor can be used to create a new LinearCombination instance which
     *  contains no operators. Operators may be added by calling `add_term`
     *  or `add_terms`.
     *
     *  @throw None No throw gurantee.
     *
     *  Complexity: Constant
     */
    LinearCombination() noexcept;

    /** @brief Creates a new LinearCombination instance by deep copying another
     *         instance.
     *
     *  This ctor can be used to create a new LinearCombination instance which
     *  contains a deep copy of @p other
     *
     *  @param[in] other LinearCombination instance to copy
     *
     *  @throw std::bad_alloc if there is insufficient memory to copy the
     *                        LinearCombination instance. Strong exception
     *                        gurantee.
     *
     *  Complexity: Linear in the number of operators which comprise @p other.
     */
    LinearCombination(const LinearCombination& other);

    /** @brief Creates a new LinearCombination instance by taking ownership of
     *         another instance's state.
     *
     *  This ctor will create a new LinearCombination instance by taking
     *  ownership of @p other's state.
     *
     *  @param[in,out] other The instance whose state will be transferred to
     *                       the resulting instance. After the operation
     *                       @p other will not contain a PIMPL and will thus not
     *                       be usable until another PIMPL-containing
     *                       LinearCombination instance is assigned to it.
     *
     *  @throw None No throw guarantee.
     *
     *  Complexity: Constant.
     */
    LinearCombination(LinearCombination&& other) noexcept;

    /** @brief Deep copy the state of another LinearCombination instance to the
     *         current instance.
     *
     *  This function will deep copy the state of @p other to the current
     *  instance. The state previously held by this instance will be released.
     *
     *  @param[in] other The LinearCombination instance to deep copy.
     *
     *  @return A reference to the current instance which contains a deep copy
     *          of @p other.
     *
     *  @throw std::bad_alloc if there is insufficient memory to copy the
     *                        LinearCombination instance @p other. Strong
     *                        exception gurantee.
     *
     *  Complexity: Linear in the number of operators which comprise @p other.
     */
    LinearCombination& operator=(const LinearCombination& other);

    /** @brief Transfer the ownership of the state of another LinearCombination
     *         instance to the current instance.
     *
     *  This function will transfer ownership of @p other's state to the
     *  current instance. The state previously held by this instance will
     *  be released.
     *
     *  @param[in,out] other The LinearCombination instance whose state is to be
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
    LinearCombination& operator=(LinearCombination&& other) noexcept;

    /// Derived implementation of polymorphic equality comparison
    bool is_equal_impl(const OperatorBase& other) const noexcept override;

    void hash_impl(chemist::detail_::Hasher& h) const override;
    std::string as_string_impl() const override;

private:
    LinearCombinationPIMPL& pimpl_();
    const LinearCombinationPIMPL& pimpl_() const;

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
    std::unique_ptr<LinearCombinationPIMPL> m_pimpl_;

}; // class LinearCombination

// -----------------------------------------------------------------------------
// ------------------------ Inline Implementations -----------------------------
// -----------------------------------------------------------------------------

template<typename OpType>
LinearCombination& LinearCombination::add_term(OpType&& op) {
    using clean_t = std::decay_t<OpType>;
    static_assert(is_operator_v<clean_t>, "Argument must be an operator.");
    const auto& rtti = typeid(clean_t);
    add_term_(rtti, std::make_shared<clean_t>(std::forward<OpType>(op)));
    return *this;
}

template<typename... OpTypes>
LinearCombination& LinearCombination::add_terms(OpTypes&&... ops) {
    if constexpr(sizeof...(OpTypes) > 0) {
        (add_term(std::forward<OpTypes>(ops)), ...);
    }
    return *this;
}

template<typename OpType>
typename LinearCombination::get_const_return_type<OpType>
LinearCombination::get_terms() const {
    static_assert(is_operator_v<OpType>, "OpType must be an operator.");
    auto type_erased_terms = get_terms_(typeid(OpType));
    get_const_return_type<OpType> ret_terms(type_erased_terms.size());
    std::transform(type_erased_terms.begin(), type_erased_terms.end(),
                   ret_terms.begin(),
                   std::dynamic_pointer_cast<const OpType, const OperatorBase>);
    return ret_terms;
}

template<typename OpType>
typename LinearCombination::get_return_type<OpType>
LinearCombination::get_terms() {
    static_assert(is_operator_v<OpType>, "OpType must be an operator.");
    auto type_erased_terms = get_terms_(typeid(OpType));
    get_return_type<OpType> ret_terms(type_erased_terms.size());
    std::transform(type_erased_terms.begin(), type_erased_terms.end(),
                   ret_terms.begin(),
                   std::dynamic_pointer_cast<OpType, OperatorBase>);
    return ret_terms;
}

} // namespace chemist::operators::detail_
