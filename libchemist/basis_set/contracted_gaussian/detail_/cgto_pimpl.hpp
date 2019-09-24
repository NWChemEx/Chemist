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
     *  @throw none No throw guarantee.
     */
    CGTOPIMPL() noexcept = default;

    CGTOPIMPL(const CGTOPIMPL<T>& rhs) = delete;
    CGTOPIMPL(CGTOPIMPL<T>&& rhs)      = delete;
    CGTOPIMPL<T>& operator=(const CGTOPIMPL<T>& rhs) = delete;
    CGTOPIMPL<T>& operator=(CGTOPIMPL<T>&& rhs) = delete;

    /// Defaulted polymorphic no-throw dtor
    virtual ~CGTOPIMPL() noexcept = default;

    /** @brief Determines the number of primitives comprising the contraction.
     *
     *  This function is used to determine the number of primitive Gaussians in
     *  the contraction. It defers to the derived class's size_ implementation.
     *
     *  @return The number of primitives in the present contraction
     *
     *  @throw none no throw guarantee.
     */
    size_type size() const noexcept { return size_(); }

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
    reference coef(size_type i) { return coef_(i); }

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
    const_reference coef(size_type i) const { return coef_(i); }

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
    reference exp(size_type i) { return exp_(i); }

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
    const_reference exp(size_type i) const { return exp_(i); }

private:
    /// Implemented by derived class to implement size
    virtual size_type size_() const noexcept = 0;
    /// Implemented by derived class to implement coef()
    virtual reference coef_(size_type i) = 0;
    /// Implemented by derived class to implement coef() const
    virtual const_reference coef_(size_type i) const = 0;
    /// Implemented by derived class to implement exp()
    virtual reference exp_(size_type i) = 0;
    /// Implemented by derived class to implement exp() const
    virtual const_reference exp_(size_type i) const = 0;
}; // class CGTOPIMPL

/** @brief Code factorization for the implementation of the CGTOPIMPL
 *
 *  The owning and aliasing versions of the CGTOPIMPL differ by whether they
 *  hold the parameters in a MathSet or a MathSetView. We use type-erasure via
 *  the CGTOPIMPL class to remove that distinction. This derived class
 *  implements the CGTOPIMPL using APIs that are valid regardless of
 *  which container is internally stored.
 *
 *  @tparam T The type used to hold the Primitive's parameters.
 *  @tparam Container The type of the container used to hold the Primitive's
 *          parameters.
 */
template<typename T, typename Container>
class CGTOPIMPL_ : public CGTOPIMPL<T> {
private:
    /// Type of the base class
    using base_type = CGTOPIMPL<T>;

public:
    /// Type of a read/write reference to a parameter
    using reference = typename base_type::reference;
    /// Type of a read-only reference to a parameter
    using const_reference = typename base_type::const_reference;
    /// Type of an index or offset
    using size_type = typename base_type::size_type;

    /** @brief Creates a PIMPL that holds no primitives.
     *
     *  The PIMPL that results from this ctor will hold no primitives. At the
     *  moment there is no way to add more primitives to it and it is
     *  essentially a placeholder.
     *
     *  @throw std::bad_alloc if their is insufficient memory to allocate the
     *         PIMPLs for the MathSet
     */
    CGTOPIMPL_() = default;

    /** @brief Initializes the PIMPL with the provided coefficients and
     *         exponents.
     *
     * @tparam U Should be T if this container owns the values and T* if they
     *           are being aliased.
     * @param[in] coefs A vector of coefficients to copy/alias
     * @param[in] exps A vector of exponents to copy/alias
     *
     * @throw std::bad_alloc if there is insufficient memory to allocate the
     *        PIMPLs for the MathSets. Strong throw guarantee.
     */
    template<typename U>
    CGTOPIMPL_(std::vector<U> coefs, std::vector<U> exps);

private:
    /// Implements size() for the base class
    size_type size_() const noexcept override { return m_coefs_.size(); }
    /// Implements coef() for the base class
    reference coef_(size_type i) override { return m_coefs_[i]; }
    /// Implements coef() const for the base class
    const_reference coef_(size_type i) const override { return m_coefs_[i]; }
    /// Implements exp() for the base class
    reference exp_(size_type i) override { return m_exps_[i]; }
    /// Implements exp() const for the base class
    const_reference exp_(size_type i) const override { return m_exps_[i]; }

    /// A MathSet or MathSetView of the coefficients
    Container m_coefs_;
    /// A MathSet or MathSetView of the exponents
    Container m_exps_;
};

/// Typedef for the PIMPL that actually owns its parameters
template<typename T>
using OwningCGTOPIMPL = CGTOPIMPL_<T, utilities::MathSet<T>>;

/// Typedef for the PIMPL that aliases its parameters
template<typename T>
using AliasingCGTOPIMPL = CGTOPIMPL_<T, utilities::MathSetView<T>>;

// ---------------------------------Implementations-----------------------------

template<typename T, typename Container>
template<typename U>
CGTOPIMPL_<T, Container>::CGTOPIMPL_(std::vector<U> coefs,
                                     std::vector<U> exps) :
  m_coefs_(coefs.begin(), coefs.end()),
  m_exps_(exps.begin(), exps.end()) {}

} // namespace libchemist::detail_
