#pragma once
#include "libchemist/operator/derived_operator.hpp"

namespace libchemist {

/** @brief A class to store and manipulate Fock operators.
 *
 *  This class provides a set of public APIs for the storage and
 *  manipulations of generic Fock operators resulting from molecular
 *  Hamiltonians representing some physical state of affairs.
 *  FockOperator instances are comprised of a collection of one Electron
 *  Operators which represent either physical interactions of the particles
 *  of the system or mean-field potentials arising from the interactions of
 *  individual Electrons with the average bevhaviour of the other Electrons.
 *
 */
struct FockOperator : public DerivedOperator {
    /** @brief Creates a new FockOperator instance containing no operators.
     *
     *  This ctor can be used to create a new FockOperator instance which
     *  contains no operators. Operators may be added by calling `add_term`
     *  or `add_terms`.
     *
     *  @throw None No throw gurantee.
     *
     *  Complexity: Constant
     */
    FockOperator() noexcept;

    /** @brief Creates a new FockOperator instance by deep copying another
     *         instance.
     *
     *  This ctor can be used to create a new FockOperator instance which
     *  contains a deep copy of @p other
     *
     *  @param[in] other FockOperator instance to copy
     *
     *  @throw std::bad_alloc if there is insufficient memory to copy the
     *                        FockOperator instance. Strong exception gurantee.
     *
     *  Complexity: Linear in the number of operators which comprise @p other.
     */
    FockOperator(const FockOperator& other);

    /** @brief Creates a new FockOperator instance by taking ownership of
     *         another instance's state.
     *
     *  This ctor will create a new FockOperator instance by taking ownership of
     *  @p other's state.
     *
     *  @param[in,out] other The instance whose state will be transferred to the
     *                       resulting instance. After the operation @p other
     * will not contain a PIMPL and will thus not be usable until another
     * PIMPL-containing FockOperator instance is assigned to it.
     *
     *  @throw None No throw guarantee.
     *
     *  Complexity: Constant.
     */
    FockOperator(FockOperator&& other) noexcept;

    /// Defaulted no-throw dtor
    virtual ~FockOperator() noexcept;

    /** @brief Deep copy the state of another FockOperator instance to the
     *         current instance.
     *
     *  This function will deep copy the state of @p other to the current
     *  instance. The state previously held by this instance will be released.
     *
     *  @param[in] other The FockOperator instance to deep copy.
     *
     *  @return A reference to the current instance which contains a deep copy
     *          of @p other.
     *
     *  @throw std::bad_alloc if there is insufficient memory to copy the
     *                        FockOperator instance @p other. Strong exception
     *                        gurantee.
     *
     *  Complexity: Linear in the number of operators which comprise @p other.
     */
    FockOperator& operator=(const FockOperator& other);

    /** @brief Transfer the ownership of the state of another FockOperator
     *         instance to the current instance.
     *
     *  This function will transfer ownership of @p other's state to the
     *  current instance. The state previously held by this instance will
     *  be released.
     *
     *  @param[in,out] other The FockOperator instance whose state is to be
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
    FockOperator& operator=(FockOperator&& other) noexcept;

    /** @brief Creates a new FockOperator instance from a collection of
     *  Operator instances.
     *
     *  This ctor can be used to create a FockOperator instance from
     *  a collection of Operator instances. Additional terms may be
     *  added by invoking `add_term` or `add_terms`.
     *
     *  @tparam Args Parameter pack of passed Operator types, must all
     *               be derived from Operator.
     *
     *  @param[in] args Operator instances from which to construct the
     * FockOperator.
     *
     *  @throw std::bad_alloc if there is insufficient memory either to create
     * the PIMPL or to store the internal state of any Operator instance. Basic
     * exception gurantee.
     *
     *  Complexity: Linear in the size of @p args
     */
    template<typename... Args,
             typename = std::enable_if_t<are_one_Electron_operators_v<Args...>>>
    FockOperator(Args&&... args) :
      DerivedOperator(std::forward<Args>(args)...) {}

    /** @brief Add an additonal operator to the FockOperator.
     *
     *  This function adds an operator to this FockOperator. If the
     *  FockOperator is in a PIMPL-less state, this function will
     *  generate a PIMPL instance.
     *
     *  @tparam OpType The strong type of the operator to add to the
     *                 FockOperator. Must be derived from Operator.
     *
     *  @param[in] op The operator to add to this FockOperator.
     *
     *  @return A reference to the current FockOperator instance.
     *
     *  @throw std::bad_alloc if there is insufficient memory either to create
     * the PIMPL or to store the internal state of the Operator instance. Basic
     * exception gurantee.
     *
     *  Complexity: Constant
     */
    template<typename OpType>
    enable_if_operator_t<OpType, FockOperator&> add_term(OpType&& op) {
        DerivedOperator::add_term(std::forward<OpType>(op));
        return *this;
    }

    /** @brief Add several additonal operators to the FockOperator.
     *
     *  This function adds several operators to this FockOperator. If the
     * FockOperator is in a PIMPL-less state, this function will generate a
     * PIMPL instance.
     *
     *  @tparam Ops The strong types of the operators to add to the
     *              FockOperator. All must be derived from Operator.
     *
     *  @param[in] ops The operators to add to this FockOperator.
     *
     *  @return A reference to the current FockOperator instance.
     *
     *  @throw std::bad_alloc if there is insufficient memory either to create
     * the PIMPL or to store the internal state of any Operator instance. Basic
     * exception gurantee.
     *
     *  Complexity: Linear in the size of @p ops.
     */
    template<typename... Ops>
    std::enable_if_t<are_one_Electron_operators_v<Ops...>, FockOperator&>
    add_terms(Ops&&... ops) {
        DerivedOperator::add_terms(std::forward<Ops>(ops)...);
        return *this;
    }

protected:
    friend class DerivedOperator;
    virtual bool is_equal_impl(
      const DerivedOperator& other) const noexcept override;

}; // class FockOperator

} // namespace libchemist
