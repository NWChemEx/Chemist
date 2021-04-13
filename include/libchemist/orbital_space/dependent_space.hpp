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
 *  @tparam SparseMapType The type of the SparseMap instance.
 *  @tparam OverlapType The type used to store the overlap matrix.
 */
template<typename SparseMapType, typename BaseType>
class DependentBaseSpace_ : public BaseType {
public:
    /// The type of the sparse map
    using sparse_map_type = SparseMapType;

    /// The type used for indexing and offsets
    using size_type = std::size_t;

    /** @brief Creates a DependentBaseSpace_ with no SparseMap or overlap
     *         matrix.
     */
    DependentBaseSpace_() = default;

    /** @brief Creates a DependentBaseSpace_ with a SparseMap, but no overlap
     *         matrix.
     *
     * @param[in] sm The SparseMap to associate with this space.
     */
    explicit DependentBaseSpace_(sparse_map_type sm) : m_sm_(std::move(sm)) {}

    /** @brief Creates a new DependentBaseSpace_ with the provided state.
     *
     * @param[in] sm The sparse map detailing which members of this dependent
     *               space are paired with members of the independent space.
     *
     * @param[in] args  The arguments to forward to the base class's ctor
     *
     * @throw ??? If `overlap_type`'s move ctor throws. Same throw guarantee as
     *            `overlap_type`'s move ctor.
     */
    template<typename... Args>
    explicit DependentBaseSpace_(sparse_map_type sm, Args&&... args);

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
    /// Should be overriden by the derived class to implement hashing
    virtual void hash_(sde::Hasher& h) const {
        h(m_sm_);
        BaseType::hash_(h);
    }

    /// Should be overrident by the derived class to implement size
    virtual size_type size_() const noexcept { return m_sm_.size(); }

private:
    /// The sparse map between the independent space and this space
    sparse_map_type m_sm_;
};

template<typename SparseMapType, typename BaseType, typename RHSMapType,
         typename RHSBaseType>
bool operator==(const DependentBaseSpace_<SparseMapType, BaseType>& lhs,
                const DependentBaseSpace_<RHSMapType, RHSBaseType>& rhs) {
    using clean_lhs_t = std::decay_t<decltype(lhs)>;
    using clean_rhs_t = std::decay_t<decltype(rhs)>;
    if constexpr(std::is_same_v<clean_rhs_t, clean_lhs_t>) {
        return sde::hash_objects(lhs) == sde::hash_objects(rhs);
    } else {
        return false;
    }
}

template<typename SparseMapType, typename BaseType, typename RHSMapType,
         typename RHSBaseType>
bool operator!=(const DependentBaseSpace_<SparseMapType, BaseType>& lhs,
                const DependentBaseSpace_<RHSMapType, RHSBaseType>& rhs) {
    return !(lhs == rhs);
}

/// DependentBaseSpace that uses ToTs for the overlap matrix
template<typename T>
using SparseDependentBase = DependentBaseSpace_<
  sparse_map::SparseMap<sparse_map::ElementIndex, sparse_map::ElementIndex>,
  SparseBase<T>>;

// -------------------------------- Implementations ----------------------------

template<typename SparseMapType, typename BaseType>
template<typename... Args>
DependentBaseSpace_<SparseMapType, BaseType>::DependentBaseSpace_(
  SparseMapType sm, Args&&... args) :
  BaseType(std::forward<Args>(args)...), m_sm_(std::move(sm)) {}

} // namespace libchemist::orbital_space
