#pragma once
#include "chemist/basis_set/ao_basis_set.hpp"

namespace chemist::detail_ {

template<typename T>
class AOBasisSetPIMPL {
private:
    /// The type of the AOBasisSet implemented by this PIMPL
    using bs_type = AOBasisSet<T>;

public:
    /// The objects comprising the Basis set
    using value_type = typename bs_type::value_type;
    /// Read-/write-able references to elements in the basis
    using reference = typename bs_type::reference;
    /// Read-only references to elements in the basis
    using const_reference = typename bs_type::const_reference;
    /// Unsigned integer type used for indexing/offsets
    using size_type = typename bs_type::size_type;

    /** @brief Returns the number of centers in this basis set.
     *
     *  @return The number of centers in this basis set.
     *
     *  @throw None No throw guarantee.
     */
    size_type size() const noexcept { return m_centers_.size(); }

    /** @brief Adds a AtomicBasisSet to the current basis set.
     *
     *  @param[in] c The center to add to this basis set.
     *
     *  @throw std::bad_alloc if the underlying vector needs to reallocate and
     *                        there is insufficient memory. Strong throw
     *                        guarantee.
     */
    void add_center(value_type c) { m_centers_.emplace_back(std::move(c)); }

    /** @brief Returns the @p i-th center in the basis set.
     *
     *  At the moment this function performs no bounds checks because it is
     *  assumed that it will be used to implement IndexableContainerBase's
     *  operator[] (which will perform the bounds check).
     *
     *  @param[in] i The index of the requested center. @p i must be in the
     *               range [0, size()).
     *  @return A read-/write-able reference to the requested center.
     *
     *  @throw None No throw guarantee.
     */
    reference& at(size_type i) { return m_centers_[i]; }

    /** @brief Returns the @p i-th center in the basis set.
     *
     *  At the moment this function performs no bounds checks because it is
     *  assumed that it will be used to implement IndexableContainerBase's
     *  operator[] (which will perform the bounds check).
     *
     *  @param[in] i The index of the requested center. @p i must be in the
     *               range [0, size()).
     *  @return A read-only reference to the requested center.
     *
     *  @throw None No throw guarantee.
     */
    const_reference& at(size_type i) const { return m_centers_[i]; }

private:
    /// The actual elements of the basis set
    std::vector<value_type> m_centers_;
}; // class AOBasisSetPIMPL

} // namespace chemist::detail_
