#pragma once
#include "libchemist/orbital_space/base_space.hpp"
#include "libchemist/sparse_map/sparse_map.hpp"

namespace libchemist::orbital_space {

/** @brief An orbital spaces that is dependent on another space.
 *
 *  Particularly in linear scaling theories we have sparsity relations
 *  associated with spaces. The DependentSpace class is the base class (it's
 *  conceptually a mix-in, except that it derives from `BaseSpace` for single
 *  inheritance purposes) for spaces with sparse maps associated with them. It
 *  adds a SparseMap instance to the orbital space's state. The independent
 *  indices of the sparse map index orbitals in the independent space whereas
 *  the dependent indices index orbitals in the orbital space which derives from
 *  DependentSpace. For example given an `AOSpace` derived from a DependentSpace
 *  the dependent indices index the AOs associated with a particular tuple of
 *  independent orbitals, whereas a `DerivedSpace` derived from a
 *  `DependentSpace` has dependent indices which index the orbitals in the
 *  `DerivedSpace` and not the "from" space.
 *
 *  @note For any pair of orbital spaces it is always possible to define a
 *        sparse map, but the resulting sparse map will be trivial (every
 *        independent tuple maps to every dependent index) unless there is
 *        enforced sparsity. The prescence of the `DependentSpace` class means
 *        that, the orbital space has a (in general) non-trivial sparse map
 *        associated with it and the user expects you to account for it.
 */
class DependentSpace : public BaseSpace {
public:
    /// The type used for indexing and offsets
    using size_type = typename BaseSpace::size_type;

    /// The type of the sparse map stored in this instance
    using sparse_map_type = type::sparse_map;

    /** @brief Creates a DependentSpace with an empty SparseMap.
     */
    DependentSpace() = default;

    /** @brief Creates a DependentSpace with a SparseMap
     *
     * @param[in] sm The SparseMap to associate with this space.
     */
    explicit DependentSpace(sparse_map_type sm) : m_sm_(std::move(sm)) {}

    /** @brief The sparse map from the independent space to this space
     *
     *  @return The internal SparseMap instance in a read/write format.
     *  @throw none No throw guarantee.
     */
    auto& sparse_map() { return m_sm_; }

    /** @brief The sparse map from the independent space to this space
     *
     *  @return The internal SparseMap instance in a read-only format.
     *  @throw none No throw guarantee.
     */
    const auto& sparse_map() const { return m_sm_; }

protected:
    /// Returns the number of dependent orbitals in this space
    virtual size_type size_() const noexcept override;

    /// Adds the hash of the sparse map to the provided hasher.
    virtual void hash_(sde::Hasher& h) const override { h(m_sm_); }

    /// Returnst true if the spaces have the same sparse map
    virtual bool equal_(const BaseSpace& rhs) const noexcept override;

private:
    /// The sparse map between the independent space and this space
    sparse_map_type m_sm_;
};

/** @brief Compares two DependentSpaces for equality.
 *
 *  @relates DependentSpace
 *
 *  Two DependentSpace instances are the same if they possess the same sparse
 *  map.
 *
 *  @param[in] lhs The dependent space on the left-side of the equality operator
 *  @param[in] rhs The dependent space on the right-side of the equality
 *                 operator.
 *
 *  @return True if @p lhs and @p rhs have sparse maps that are equal and false
 *          otherwise.
 *
 *  @throw None No throw guarantee.
 */
inline bool operator==(const DependentSpace& lhs, const DependentSpace& rhs) {
    return lhs.sparse_map() == rhs.sparse_map();
}

/** @brief Determines if two DependentSpaces are different.
 *
 *  @relates DependentSpace
 *
 *  Two DependentSpace instances are the same if they possess the same sparse
 *  map.
 *
 *  @param[in] lhs The dependent space on the left-side of the inequality
 *                 operator.
 *  @param[in] rhs The dependent space on the right-side of the inequality
 *                 operator.
 *
 *  @return False if @p lhs and @p rhs have sparse maps that are equal and true
 *          otherwise.
 *
 *  @throw None No throw guarantee.
 */
inline bool operator!=(const DependentSpace& lhs, const DependentSpace& rhs) {
    return !(lhs == rhs);
}

// -------------------------------- Implementations ----------------------------

inline typename DependentSpace::size_type DependentSpace::size_()
  const noexcept {
    size_type n = 0;
    // This only works b/c m_sm_ is element2element
    for(const auto& [i_idx, domain] : m_sm_) n += domain.size();
    return n;
}

inline bool DependentSpace::equal_(const BaseSpace& rhs) const noexcept {
    return equal_common(*this, rhs);
}

} // namespace libchemist::orbital_space
