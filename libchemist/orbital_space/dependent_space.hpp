#pragma once
#include "libchemist/orbital_space/base_space.hpp"
#include "libchemist/sparse_map/sparse_map.hpp"

namespace libchemist::orbital_space {

/** @brief Base class for orbital spaces that are dependent on another space.
 *
 *  Particularly in linear scaling theories we often get orbital spaces that
 *  depend on another space. That is to say we get a different set of orbitals
 *  for each orbital (or tuple of orbitals) in the independent space.
 *
 *  @tparam OverlapType The type used to store the overlap matrix.
 */
template<typename OverlapType>
class DependentBaseSpace_ {
public:
    /// The type of the sparse map
    using sparse_map_type = sparse_map::SparseMap;

    /// The type used to store the overlap matrix
    using overlap_type = OverlapType;

    DependentBaseSpace_() = default;

    DependentBaseSpace_(sparse_map_type sm) : m_sm_(std::move(sm)) {}

    /** @brief Creates a new DependentBaseSpace_ with the provided state.
     *
     * @param[in] sm The sparse map detailing which members of this dependent
     *               space are paired with members of the independent space.
     *
     * @param[in] S  The overlap matrix of this orbital space.
     *
     * @throw ??? If `overlap_type`'s move ctor throws. Same throw guarantee as
     *            `overlap_type`'s move ctor.
     */
    DependentBaseSpace_(sparse_map_type sm, overlap_type S);

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

    /** @brief The overlap matrix of this space.
     *
     *  @return The overlap matrix in a read/write format.
     */
    auto& S() { return m_space_.S(); }

    /** @brief The overlap matrix of this space.
     *
     *  @return The overlap matrix in a read-only format.
     */
    const auto& S() const { return m_space_.S(); }

    /** @brief Returns a hash of this orbital space's state.
     *
     *  It should be noted that the hash also takes into account all of the
     *  derived class's state (if any).
     *
     *  @param[in, out] h The instance hashing the state. After this call its
     *                    internal hash will be updated with the state of the
     *                    sparse map and the overlap matrix.
     */
    void hash(sde::Hasher& h) const { hash_(h); }

protected:
    /// Should be overriden by the derived class to implement hashing
    virtual void hash_(sde::Hasher& h) const { h(m_sm_, m_space_); }

private:
    /// The sparse map between the independent space and this space
    sparse_map_type m_sm_;

    /// The instance managing the orbital space part of this space
    BaseSpace_<overlap_type> m_space_;
};

template<typename OverlapType>
bool operator==(const DependentBaseSpace_<OverlapType>& lhs,
                const DependentBaseSpace_<OverlapType>& rhs) {
    return (sde::hash_objects(lhs.S()) == sde::hash_objects(rhs.S())) &&
           lhs.sparse_map() == rhs.sparse_map();
}

template<typename OverlapType>
bool operator!=(const DependentBaseSpace_<OverlapType>& lhs,
                const DependentBaseSpace_<OverlapType>& rhs) {
    return !(lhs == rhs);
}

/// DependentBaseSpace that uses ToTs for the overlap matrix
template<typename T>
using SparseDependentBase = DependentBaseSpace_<type::tensor_of_tensors<T>>;

// -------------------------------- Implementations ----------------------------

template<typename OverlapType>
DependentBaseSpace_<OverlapType>::DependentBaseSpace_(sparse_map::SparseMap sm,
                                                      overlap_type S) :
  m_sm_(std::move(sm)), m_space_(std::move(S)) {}

} // namespace libchemist::orbital_space
