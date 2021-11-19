#pragma once
#include "../primitive/primitive_pimpl.hpp"
#include <memory>
#include <utilities/containers/own_or_borrow.hpp>
#include <vector>

namespace chemist::detail_ {

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
    /// Unsigned integral type used for indexing and offsets
    using size_type = std::size_t;
    /// Type used to store the primitives' parameters
    using param_type = T;
    /// Type used to store the parameters for the set of primitives
    using param_set = std::vector<T>;

    /** @brief Creates a new CGTOPIMPL which has no Primitives.
     *
     *  This ctor is used to create a new CGTOPIMPL instance which
     *  contains no Primitives.
     *
     *  @throw std::bad_alloc if there is insufficient memory to allocate the
     *         MathSet's PIMPLs. Strong throw guarantee.
     */
    CGTOPIMPL() = default;

    CGTOPIMPL(const CGTOPIMPL<T>& rhs);
    CGTOPIMPL(CGTOPIMPL<T>&& rhs) = delete;
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

    std::unique_ptr<detail_::PrimitivePIMPL<T>> at(size_type i) const;

private:
    /// The contraction coefficients for this CGTO
    utilities::OwnOrBorrow<param_set> m_cs_;
    /// The exponents of each primitive in this CGTO
    utilities::OwnOrBorrow<param_set> m_as_;
}; // class CGTOPIMPL

// ------------------------------Implementations--------------------------------

template<typename T>
template<typename U, typename V>
CGTOPIMPL<T>::CGTOPIMPL(U&& cs, V&& as) noexcept :
  m_cs_(std::forward<U>(cs)), m_as_(std::forward<V>(as)) {}

template<typename T>
CGTOPIMPL<T>::CGTOPIMPL(const CGTOPIMPL<T>& rhs) :
  m_cs_(rhs.m_cs_.value()), m_as_(rhs.m_as_.value()) {}

template<typename T>
std::unique_ptr<detail_::PrimitivePIMPL<T>> CGTOPIMPL<T>::at(
  size_type i) const {
    return std::make_unique<detail_::PrimitivePIMPL<T>>(
      const_cast<T*>(&m_cs_.value()[i]), const_cast<T*>(&m_as_.value()[i]));
}

} // namespace chemist::detail_
