#pragma once
#include <utilities/containers/math_set/math_set.hpp>
#include <utilities/containers/own_or_borrow.hpp>

namespace libchemist::detail_ {

/** @brief Defines the API for the PIMPLs used to implement the
 *         ContractedGaussian and the ContractedGaussianView classes.
 *
 *  This class defines the API that all PIMPLs used to implement a
 *  ContractedGaussian must obey. The derived class is responsible for
 *  implementing the methods for getting the number of primitives and the
 *  associated parameters.
 *
 *  @tparam T The type used to store the CGTO's parameters. Should be non-cv
 *          qualified and is expected to be a POD floating point type.
 */
template<typename T>
class CGTOPIMPL {
public:
    /// Type of a read/write reference to a primitive in the CGTO
    using reference = T&;
    /// Type of a read-only reference to a primitive in the CGTO
    using const_reference = const T&;
    /// Type used for offsets and indices
    using size_type = std::size_t;

    /** @brief Creates a new CGTOPIMPL which has no Primitives.
     *
     *  This ctor is used to create a new CGTOPIMPL instance which
     *  contains no Primitives.
     *
     *  @throw std::bad_alloc if there is insufficient memory to allocate the
     *         MathSet's PIMPLs. Strong throw guarantee.
     */
    CGTOPIMPL() = default;

    CGTOPIMPL(const CGTOPIMPL<T>& rhs) = delete;
    CGTOPIMPL(CGTOPIMPL<T>&& rhs)      = delete;
    CGTOPIMPL<T>& operator=(const CGTOPIMPL<T>& rhs) = delete;
    CGTOPIMPL<T>& operator=(CGTOPIMPL<T>&& rhs) = delete;

    /** @brief Creates a new CGTOPIMPL using the provided coefficients and
     *         exponents.
     *
     * @tparam U Assumed to be MathSet<T> or MathSet<T>*
     * @tparam V Assumed to be MathSet<T> or MathSet<T>*
     * @param[in] cs The set of coefficients this PIMPL will be associated with.
     * @param[in] as The set of exponents that this PIMPL will contain.
     *
     * @throw none No throw guarantee.
     */
    template<typename U, typename V>
    CGTOPIMPL(U&& cs, V&& as)
    noexcept;

    /** @brief Determines the number of primitives comprising the contraction.
     *
     *  This function is used to determine the number of primitive Gaussians in
     *  the contraction. It defers to the derived class's size_ implementation.
     *
     *  @return The number of primitives in the present contraction
     *
     *  @throw none no throw guarantee.
     */
    size_type size() const noexcept { return m_cs_.value().size(); }

    /** @brief Returns the contraction coefficient of the @p i-th primitive.
     *
     *  This function is used to retrieve the contraction coefficient for a
     *  particular primitive involved in the contraction.
     *
     *  @param[in] i The  primitive whose contraction coefficient is wanted.
     *               Must be in the range [0, size()).
     *
     * @return The requested contraction coefficient by read/write reference.
     *
     * @throw std::out_of_range if @p i is not in the range [0, size()). Strong
     *        throw guarantee.
     */
    reference coef(size_type i) { return m_cs_.value()[i]; }

    /** @brief Returns the contraction coefficient of the @p i-th primitive.
     *
     *  This function is used to retrieve the contraction coefficient for a
     *  particular primitive involved in the contraction. The resulting
     *  reference is read-only.
     *
     *  @param[in] i The  primitive whose contraction coefficient is wanted.
     *               Must be in the range [0, size()).
     *
     * @return The requested contraction coefficient by read-only reference.
     *
     * @throw std::out_of_range if @p i is not in the range [0, size()). Strong
     *        throw guarantee.
     */
    const_reference coef(size_type i) const { return m_cs_.value()[i]; }

    /** @brief Returns the exponent of the @p i-th primitive.
     *
     *  This function is used to retrieve the exponent for a particular
     *  primitive involved in the contraction.
     *
     *  @param[in] i The  primitive whose exponent is wanted. Must be in the
     *               range [0, size()).
     *
     * @return The requested exponent by read/write reference.
     *
     * @throw std::out_of_range if @p i is not in the range [0, size()). Strong
     *        throw guarantee.
     */
    reference exp(size_type i) { return m_as_.value()[i]; }

    /** @brief Returns the exponent of the @p i-th primitive.
     *
     *  This function is used to retrieve the exponent for a particular
     *  primitive involved in the contraction.
     *
     *  @param[in] i The  primitive whose exponent is wanted. Must be in the
     *               range [0, size()).
     *
     * @return The requested exponent by read-only reference.
     *
     * @throw std::out_of_range if @p i is not in the range [0, size()). Strong
     *        throw guarantee.
     */
    const_reference exp(size_type i) const { return m_as_.value()[i]; }

private:
    /// The contraction coefficients for this CGTO
    utilities::OwnOrBorrow<utilities::MathSet<T>> m_cs_;
    /// The exponents of each primitive in this CGTO
    utilities::OwnOrBorrow<utilities::MathSet<T>> m_as_;
}; // class CGTOPIMPL

// ------------------------------Implementations--------------------------------

template<typename T>
template<typename U, typename V>
CGTOPIMPL<T>::CGTOPIMPL(U&& cs, V&& as) noexcept :
  m_cs_(std::forward<U>(cs)),
  m_as_(std::forward<V>(as)) {}

} // namespace libchemist::detail_
