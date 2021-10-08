#pragma once
#include "libchemist/operators/detail_/linear_combination.hpp"
namespace libchemist::operators {

/** @brief A class to store and manipulate molecular Hamiltonians.
 *
 *  The
 */
class Hamiltonian_Nuclear : public detail_::LinearCombination {
private:
    using base_type = detail_::LinearCombination;

    template<typename T>
    static constexpr bool is_me_v = std::is_same_v<Hamiltonian_Nuclear, T>;

    template<typename T>
    using disable_if_me_t = std::enable_if_t<!is_me_v<T>>;

public:
    using size_type = std::size_t;

    /** @brief Creates a new Hamiltonian derivative instance containing no operators.
     *
     *  This ctor can be used to create a new Hamiltonian derivative instance which
     *  contains no operators. Operators may be added by calling `add_term`
     *  or `add_terms`.
     *
     *  @throw None No throw guarantee.
     *
     *  Complexity: Constant
     */
    Hamiltonian_Nuclear() noexcept;

    /** @brief Creates a new Hamiltonian derivative instance by deep copying another
     *         instance.
     *
     *  This ctor can be used to create a new Hamiltonian derivative instance which
     *  contains a deep copy of @p other
     *
     *  @param[in] other Hamiltonian derivative instance to copy
     *
     *  @throw std::bad_alloc if there is insufficient memory to copy the
     *                        Hamiltonian derivative instance. Strong exception
     *                        guarantee.
     *
     *  Complexity: Linear in the number of operators which comprise @p
     *              other.
     */
    Hamiltonian_Nuclear(const Hamiltonian_Nuclear& other);

    /** @brief Creates a new Hamiltonian derivative instance by taking ownership of
     *         another instance's state.
     *
     *  This ctor will create a new Hamiltonian derivative instance by taking ownership
     *  of @p other's state.
     *
     *  @param[in,out] other The instance whose state will be transferred to
     *                       the resulting instance. After the operation @p other
     * will not contain a PIMPL and will thus not be usable until another
     * PIMPL-containing Hamiltonian instance is assigned to it.
     *
     *  @throw None No throw guarantee.
     *
     *  Complexity: Constant.
     */
    Hamiltonian_Nuclear(Hamiltonian_Nuclear&& other) noexcept;

    /// Defaulted no-throw dtor
    virtual ~Hamiltonian_Nuclear() noexcept;

    /** @brief Deep copy the state of another Hamiltonian derivative instance to the
     *         current instance.
     *
     *  This function will deep copy the state of @p other to the current
     *  instance. The state previously held by this instance will be
     *  released.
     *
     *  @param[in] other The Hamiltonian derivative instance to deep copy.
     *
     *  @return A reference to the current instance which contains a deep
     *          copy of @p other.
     *
     *  @throw std::bad_alloc if there is insufficient memory to copy the
     *                        Hamiltonian derivative instance @p other. Strong exception
     *                        guarantee.
     *
     *  Complexity: Linear in the number of operators which comprise @p
     *              other.
     */
    Hamiltonian_Nuclear& operator=(const Hamiltonian_Nuclear& other);

    /** @brief Transfer the ownership of the state of another Hamiltonian
     *         derivative instance to the current instance.
     *
     *  This function will transfer ownership of @p other's state to the
     *  current instance. The state previously held by this instance will
     *  be released.
     *
     *  @param[in,out] other The Hamiltonian derivative instance whose state is to be
     *                       taken. @p other will no longer contain a valid
     *                       PIMPL and will need to be reassigned prior to
     *                       future usage.
     *
     *  @return The current instance after taking ownership of @p other's
     *          state.
     *
     *  @throw None No throw guarantee.
     *
     *  Complexity: Constant.
     */
    Hamiltonian_Nuclear& operator=(Hamiltonian_Nuclear&& other) noexcept;

    /** @brief Creates a new Hamiltonian derivative instance from a collection of
     *  Operator instances.
     *
     *  This ctor can be used to create a Hamiltonian derivative instance from
     *  a collection of Operator instances. Additional terms may be
     *  added by invoking `add_term` or `add_terms`.
     *
     *  @tparam Args Parameter pack of passed Operator types, must all
     *               be derived from Operator.
     *
     *  @param[in] args Operator instances from which to construct the
     *                  Hamiltonian derivative.
     *
     *  @throw std::bad_alloc if there is insufficient memory either to
     *  create the PIMPL or to store the internal state of any Operator instance.
     *  Basic exception guarantee.
     *
     *  Complexity: Linear in the size of @p args
     */
    template<typename OpType, typename... Args,
             typename = disable_if_me_t<std::decay_t<OpType>>>
    explicit Hamiltonian_Nuclear(OpType&& op0, Args&&... args);

    size_type nelectrons() const noexcept;

protected:
    bool is_equal_impl(const OperatorBase& other) const noexcept override;
    std::unique_ptr<OperatorBase> clone_impl() const override;
}; // class Hamiltonian_Nuclear

template<typename OpType, typename... Args, typename>
Hamiltonian_Nuclear::Hamiltonian_Nuclear(OpType&& op0, Args&&... args) : base_type() {
    add_terms(std::forward<OpType>(op0), std::forward<Args>(args)...);
}

} // namespace libchemist::operators
