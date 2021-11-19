#pragma once
#include <utilities/containers/own_or_borrow.hpp>

namespace chemist::detail_ {

/** @brief Implements the Primitive and PrimitiveView classes
 *
 *  The PrimitivePIMPL class is responsible for implementing the Primitive and
 *  PrimitiveView classes. Within the PrimitivePIMPL class the dispatching
 *  between owning and aliasing is handled by the OwnOrBorrow class.
 *
 *  @tparam T The type used to store the primitive's parameters. Should be
 *            non-cv qualified and is expected to be a POD floating point type.
 */
template<typename T>
class PrimitivePIMPL {
public:
    /// Type of a read/write reference to one of this Primitive's parameters
    using reference = T&;
    /// Type of a read-only reference to one of this Primitive's parameters
    using const_reference = const T&;

    /** @brief Creates a new PrimitivePIMPL which has a coefficient and exponent
     *         of 0.0.
     *
     *  This ctor is used to create a new PrimitivePIMPL instance in a default
     *  state. The default state is defined as owning its memory and having a
     *  coefficient and exponent of 0.0.
     *
     *  @throw none No throw guarantee.
     */
    PrimitivePIMPL() noexcept = default;

    /** @brief Makes a deep copy of another PrimitivePIMPL
     *
     *  This ctor will create a new PrimitivePIMPL by deep copying the state of
     *  another instance.
     *
     * @param[in] rhs The PrimitivePIMPL instance to copy.
     *
     * @throw none No throw guarantee (all memory is allocated on the stack).
     */
    PrimitivePIMPL(const PrimitivePIMPL<T>& rhs) noexcept;

    /** @brief Takes ownership of another PrimitivePIMPL's state.
     *
     *  Assuming @p T is a POD type, then this ctor will actually deep copy
     *  @p rhs's state if @p rhs owns its state or shallow copy @p rhs's state
     *  if @p rhs aliases. In turn the resulting instance will either be a deep
     *  copy of @p rhs or will alias the same Primitive as @p rhs.
     *
     * @param[in,out] rhs The instance to take the state of. After this
     *                operation @p rhs is in the same state as before the ctor.
     *
     * @throw none No throw guarantee.
     */
    PrimitivePIMPL(PrimitivePIMPL<T>&& rhs) noexcept = default;

    /** @brief Makes this instance a deep copy of another PrimitivePIMPL
     *
     *  This function will make the current instance contain a deep copy of the
     *  state of @p rhs.
     *
     * @param[in] rhs The PrimitivePIMPL instance to copy.
     *
     * @return The current instance containing a deep copy of @p rhs's state.
     *
     * @throw none No throw guarantee (all memory is allocated on the stack).
     */
    PrimitivePIMPL<T>& operator=(const PrimitivePIMPL<T>& rhs) noexcept;

    /** @brief Makes the current instance take ownership of another
     *         PrimitivePIMPL's state.
     *
     *  Assuming @p T is a POD type, then this function will actually deep copy
     *  @p rhs's state if @p rhs owns its state or shallow copy @p rhs's state
     *  if @p rhs aliases. In turn the this instance will either be a deep
     *  copy of @p rhs or will alias the same Primitive as @p rhs.
     *
     * @param[in,out] rhs The instance to take the state of. After this
     *                operation @p rhs is in the same state as before the call.
     *
     * @return The current instance containing @p rhs's state.
     *
     * @throw none No throw guarantee.
     */
    PrimitivePIMPL<T>& operator=(PrimitivePIMPL<T>&& rhs) noexcept = default;

    /** @brief Creates a new PrimitivePIMPL with the specified state.
     *
     *  This ctor is used to create a new PrimitivePIMPL with the specified
     *  state. If the inputs are by value or reference the resulting instance
     *  will contain a deep copy of the parameters. If the inputs are pointers,
     *  then the resulting instance will alias the provided state.
     *
     *  @tparam U The type of the coefficient. Expected to either be T or T*
     *  @tparam V The type of the exponent. Expected to be either T or T*.
     *
     *  @param[in] c The value of the coefficient.
     *  @param[in] a The value of the exponent.
     *
     *  @throw none No throw guarantee.
     */
    template<typename U, typename V>
    PrimitivePIMPL(U&& c, V&& a) noexcept;

    /** @brief Returns the value of the coefficient.
     *
     *  This function is used to retrieve the value of the coefficient stored
     *  in the PIMPL. The resulting instance is read/write.
     *
     *  @return A read/write reference to the coefficient.
     *
     *  @throw none No throw guarantee.
     */
    reference coef() noexcept { return m_coef_.value(); }

    /** @brief Returns the value of the coefficient.
     *
     *  This function is used to retrieve the value of the coefficient stored
     *  in the PIMPL. The resulting instance is read-only.
     *
     *  @return A read-only reference to the coefficient.
     *
     *  @throw none No throw guarantee.
     */
    const_reference coef() const noexcept { return m_coef_.value(); }

    /** @brief Returns the value of the exponent.
     *
     *  This function is used to retrieve the value of the exponent stored
     *  in the PIMPL. The resulting instance is read/write.
     *
     *  @return A read/write reference to the exponent.
     *
     *  @throw none No throw guarantee.
     */
    reference exp() noexcept { return m_exp_.value(); }

    /** @brief Returns the value of the exponent.
     *
     *  This function is used to retrieve the value of the exponent stored
     *  in the PIMPL. The resulting instance is read-only.
     *
     *  @return A read-only reference to the exponent.
     *
     *  @throw none No throw guarantee.
     */
    const_reference exp() const noexcept { return m_exp_.value(); }

private:
    /// The coefficient/weight of the Primitive
    utilities::OwnOrBorrow<T> m_coef_;
    /// The exponent of the Primitive
    utilities::OwnOrBorrow<T> m_exp_;
}; // class PrimitivePIMPL

// --------------------------------Implementations------------------------------

template<typename T>
PrimitivePIMPL<T>::PrimitivePIMPL(const PrimitivePIMPL<T>& rhs) noexcept :
  PrimitivePIMPL(rhs.coef(), rhs.exp()) {}

template<typename T>
PrimitivePIMPL<T>& PrimitivePIMPL<T>::operator=(
  const PrimitivePIMPL<T>& rhs) noexcept {
    m_coef_ = rhs.coef();
    m_exp_  = rhs.exp();
    return *this;
}

template<typename T>
template<typename U, typename V>
PrimitivePIMPL<T>::PrimitivePIMPL(U&& c, V&& a) noexcept :
  m_coef_(std::forward<U>(c)), m_exp_(std::forward<V>(a)) {}

} // namespace chemist::detail_
