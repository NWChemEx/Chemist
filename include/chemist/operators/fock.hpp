#pragma once
#include "chemist/operators/detail_/linear_combination.hpp"

namespace chemist::operators {

/** @brief A class to store and manipulate Fock operators.
 *
 *  This class provides a set of public APIs for the storage and
 *  manipulations of generic Fock operators resulting from molecular
 *  Hamiltonians representing some physical state of affairs.
 *  Fock instances are comprised of a collection of one Electron
 *  Operators which represent either physical interactions of the particles
 *  of the system or mean-field potentials arising from the interactions of
 *  individual Electrons with the average bevhaviour of the other Electrons.
 *
 */
class Fock : public detail_::LinearCombination {
private:
    using base_type = detail_::LinearCombination;

    template<typename T>
    constexpr static bool is_me_v = std::is_same_v<Fock, T>;

    template<typename T>
    using disable_if_me_t = std::enable_if_t<!is_me_v<T>>;

public:
    /** @brief Creates a new Fock instance containing no operators.
     *
     *  This ctor can be used to create a new Fock instance which
     *  contains no operators. Operators may be added by calling `add_term`
     *  or `add_terms`.
     *
     *  @throw None No throw gurantee.
     *
     *  Complexity: Constant
     */
    Fock() noexcept;

    /** @brief Creates a new Fock instance by deep copying another
     *         instance.
     *
     *  This ctor can be used to create a new Fock instance which
     *  contains a deep copy of @p other
     *
     *  @param[in] other Fock instance to copy
     *
     *  @throw std::bad_alloc if there is insufficient memory to copy the
     *                        Fock instance. Strong exception gurantee.
     *
     *  Complexity: Linear in the number of operators which comprise @p other.
     */
    Fock(const Fock& other);

    /** @brief Creates a new Fock instance by taking ownership of
     *         another instance's state.
     *
     *  This ctor will create a new Fock instance by taking ownership of
     *  @p other's state.
     *
     *  @param[in,out] other The instance whose state will be transferred to the
     *                       resulting instance. After the operation @p other
     * will not contain a PIMPL and will thus not be usable until another
     * PIMPL-containing Fock instance is assigned to it.
     *
     *  @throw None No throw guarantee.
     *
     *  Complexity: Constant.
     */
    Fock(Fock&& other) noexcept;

    /// Defaulted no-throw dtor
    virtual ~Fock() noexcept;

    /** @brief Deep copy the state of another Fock instance to the
     *         current instance.
     *
     *  This function will deep copy the state of @p other to the current
     *  instance. The state previously held by this instance will be released.
     *
     *  @param[in] other The Fock instance to deep copy.
     *
     *  @return A reference to the current instance which contains a deep copy
     *          of @p other.
     *
     *  @throw std::bad_alloc if there is insufficient memory to copy the
     *                        Fock instance @p other. Strong exception
     *                        gurantee.
     *
     *  Complexity: Linear in the number of operators which comprise @p other.
     */
    Fock& operator=(const Fock& other);

    /** @brief Transfer the ownership of the state of another Fock
     *         instance to the current instance.
     *
     *  This function will transfer ownership of @p other's state to the
     *  current instance. The state previously held by this instance will
     *  be released.
     *
     *  @param[in,out] other The Fock instance whose state is to be
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
    Fock& operator=(Fock&& other) noexcept;

    /** @brief Creates a new Fock instance from a collection of
     *  Operator instances.
     *
     *  This ctor can be used to create a Fock instance from
     *  a collection of Operator instances. Additional terms may be
     *  added by invoking `add_term` or `add_terms`.
     *
     *  @tparam Args Parameter pack of passed Operator types, must all
     *               be derived from Operator.
     *
     *  @param[in] args Operator instances from which to construct the
     * Fock.
     *
     *  @throw std::bad_alloc if there is insufficient memory either to create
     * the PIMPL or to store the internal state of any Operator instance. Basic
     * exception gurantee.
     *
     *  Complexity: Linear in the size of @p args
     */
    template<typename OpType, typename... Args,
             typename = disable_if_me_t<std::decay_t<OpType>>>
    explicit Fock(OpType&& op0, Args&&... args);

protected:
    bool is_equal_impl(const OperatorBase& other) const noexcept override;
    std::unique_ptr<OperatorBase> clone_impl() const override;
}; // class Fock

template<typename OpType, typename... Args, typename>
Fock::Fock(OpType&& op0, Args&&... args) : base_type() {
    add_terms(std::forward<OpType>(op0), std::forward<Args>(args)...);
}

} // namespace chemist::operators
