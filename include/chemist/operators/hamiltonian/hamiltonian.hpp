#pragma once
#include "chemist/operators/detail_/linear_combination.hpp"
namespace chemist::operators {

/** @brief A class to store and manipulate molecular Hamiltonians.
 *
 *  The
 */
class Hamiltonian : public detail_::LinearCombination {
private:
    using base_type = detail_::LinearCombination;

    template<typename T>
    static constexpr bool is_me_v = std::is_same_v<Hamiltonian, T>;

    template<typename T>
    using disable_if_me_t = std::enable_if_t<!is_me_v<T>>;

public:
    using size_type = std::size_t;

    /** @brief Creates a new Hamiltonian instance containing no operators.
     *
     *  This ctor can be used to create a new Hamiltonian instance which
     *  contains no operators. Operators may be added by calling `add_term`
     *  or `add_terms`.
     *
     *  @throw None No throw gurantee.
     *
     *  Complexity: Constant
     */
    Hamiltonian() noexcept;

    /** @brief Creates a new Hamiltonian instance by deep copying another
     *         instance.
     *
     *  This ctor can be used to create a new Hamiltonian instance which
     *  contains a deep copy of @p other
     *
     *  @param[in] other Hamiltonian instance to copy
     *
     *  @throw std::bad_alloc if there is insufficient memory to copy the
     *                        Hamiltonian instance. Strong exception
     gurantee.
     *
     *  Complexity: Linear in the number of operators which comprise @p
     other.
     */
    Hamiltonian(const Hamiltonian& other);

    /** @brief Creates a new Hamiltonian instance by taking ownership of
     *         another instance's state.
     *
     *  This ctor will create a new Hamiltonian instance by taking ownership
     of
     *  @p other's state.
     *
     *  @param[in,out] other The instance whose state will be transferred to
     the
     *                       resulting instance. After the operation @p other
     * will not contain a PIMPL and will thus not be usable until another
     * PIMPL-containing Hamiltonian instance is assigned to it.
     *
     *  @throw None No throw guarantee.
     *
     *  Complexity: Constant.
     */
    Hamiltonian(Hamiltonian&& other) noexcept;

    /// Defaulted no-throw dtor
    virtual ~Hamiltonian() noexcept;

    /** @brief Deep copy the state of another Hamiltonian instance to the
     *         current instance.
     *
     *  This function will deep copy the state of @p other to the current
     *  instance. The state previously held by this instance will be
     released.
     *
     *  @param[in] other The Hamiltonian instance to deep copy.
     *
     *  @return A reference to the current instance which contains a deep
     copy
     *          of @p other.
     *
     *  @throw std::bad_alloc if there is insufficient memory to copy the
     *                        Hamiltonian instance @p other. Strong exception
     *                        gurantee.
     *
     *  Complexity: Linear in the number of operators which comprise @p
     other.
     */
    Hamiltonian& operator=(const Hamiltonian& other);

    /** @brief Transfer the ownership of the state of another Hamiltonian
     *         instance to the current instance.
     *
     *  This function will transfer ownership of @p other's state to the
     *  current instance. The state previously held by this instance will
     *  be released.
     *
     *  @param[in,out] other The Hamiltonian instance whose state is to be
     *                       taken. @p other will no longer contain a valid
     *                       PIMPL and will need to be reassigned prior to
     *                       future usage.
     *
     *  @return The current instance after taking ownership of @p other's
     state.
     *
     *  @throw None No throw guarantee.
     *
     *  Complexity: Constant.
     */
    Hamiltonian& operator=(Hamiltonian&& other) noexcept;

    /** @brief Creates a new Hamiltonian instance from a collection of
     *  Operator instances.
     *
     *  This ctor can be used to create a Hamiltonian instance from
     *  a collection of Operator instances. Additional terms may be
     *  added by invoking `add_term` or `add_terms`.
     *
     *  @tparam Args Parameter pack of passed Operator types, must all
     *               be derived from Operator.
     *
     *  @param[in] args Operator instances from which to construct the
     * Hamiltonian.
     *
     *  @throw std::bad_alloc if there is insufficient memory either to
     create
     * the PIMPL or to store the internal state of any Operator instance.
     Basic
     * exception gurantee.
     *
     *  Complexity: Linear in the size of @p args
     */
    template<typename OpType, typename... Args,
             typename = disable_if_me_t<std::decay_t<OpType>>>
    explicit Hamiltonian(OpType&& op0, Args&&... args);

    size_type nelectrons() const noexcept;

protected:
    bool is_equal_impl(const OperatorBase& other) const noexcept override;
    std::unique_ptr<OperatorBase> clone_impl() const override;
}; // class Hamiltonian

template<typename OpType, typename... Args, typename>
Hamiltonian::Hamiltonian(OpType&& op0, Args&&... args) : base_type() {
    add_terms(std::forward<OpType>(op0), std::forward<Args>(args)...);
}

} // namespace chemist::operators
