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

    virtual ~CGTOPIMPL() noexcept = default;

    size_type size() const noexcept { return size_(); }
    reference coef(size_type i) { return coef_(i); }
    const_reference coef(size_type i) const { return coef_(i); }
    reference exp(size_type i) { return exp_(i); }
    const_reference exp(size_type i) const { return exp_(i); }

private:
    virtual size_type size_() const noexcept         = 0;
    virtual reference coef_(size_type i)             = 0;
    virtual const_reference coef_(size_type i) const = 0;
    virtual reference exp_(size_type i)              = 0;
    virtual const_reference exp_(size_type i) const  = 0;
}; // class CGTOPIMPL

/** @brief Code factorization for the implementation of the CGTOPIMPL
 *
 *  The owning and aliasing versions of the CGTOPIMPL differ by whether they
 *  hold the parameters in a MathSet or a MathSetView. We use type-erasure to
 *  remove that distinction. This class implements the CGTOPIMPL using
 *  duck-typing that is valid regardless of which container is internally
 *  stored.
 *
 *  @tparam T The type used to hold the Primitive's parameters.
 *  @tparam Container The type of the container used to hold the Primitive's
 *          parameters.
 */
template<typename T, typename Container>
class CGTOPIMPL_ : public CGTOPIMPL<T> {
public:
    CGTOPIMPL_(Container coefs, Container exps);

private:
    size_type size_() const noexcept override { return m_coefs_.size(); }
    reference coef_(size_type i) override { return m_coefs_[i]; }
    const_reference coef_(size_type i) const override { return m_coefs_[i]; }
    reference exp_(size_type i) override { return m_exps_[i]; }
    const_reference exp_(size_type i) const override { return m_exps_[i]; }

    Container m_coefs_;
    Container m_exps_;
};

template<typename T>
using OwningCGTOPIMPL = CGTOPIMPL_<T, utilities::MathSet<T>>;

template<typename T>
using AliasingCGTOPIMPL = CGTOPIMPL_<T, utilities::MathSetView<T>>;

} // namespace libchemist::detail_
