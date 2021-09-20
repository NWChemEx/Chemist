#pragma once
#include "libchemist/orbital_space/canonical_space.hpp"
#include "libchemist/sparse_map/sparse_map.hpp"

namespace libchemist::orbital_space {

/** @brief An orbital space which serves as an independent space for one or
 *         more sparse maps.
 *
 */
template<typename BaseType>
class IndependentSpace : public BaseType {
public:
    /// The type used for indexing and offsets
    using size_type = typename BaseType::size_type;

    /// The type of the sparse map stored in this instance
    using sparse_map_type = type::sparse_map;

    /// How the sparse maps are differentiated
    using sparse_map_register = std::map<std::string, sparse_map_type>;

    explicit IndependentSpace(BaseType base, sparse_map_register sms = {});

    template<typename DepSpace>
    void add_sparse_map(const DepSpace& dep, sparse_map_type sm);

    template<typename DepSpace>
    auto count(const DepSpace& dep) const {
        return m_sm_.count(key_(dep));
    }

    /** @brief The sparse map from the independent space to this space
     *
     *  @return The internal SparseMap instance in a read/write format.
     *  @throw none No throw guarantee.
     */
    template<typename DepSpace>
    auto& sparse_map(const DepSpace& dep) {
        return m_sm_.at(key_(dep));
    }

    /** @brief The sparse map from this space to the
     *
     *  @return The internal SparseMap instance in a read-only format.
     *  @throw none No throw guarantee.
     */
    template<typename DepSpace>
    const auto& sparse_map(const DepSpace& dep) const {
        return m_sm_.at(key_(dep));
    }

protected:
    template<typename DepSpace>
    static std::string key_(const DepSpace& dep);

    /// Adds the hash of the sparse map to the provided hasher.
    virtual void hash_(pluginplay::Hasher& h) const override { h(m_sm_); }

    /// Returnst true if the spaces have the same sparse map
    virtual bool equal_(const BaseSpace& rhs) const noexcept override;

private:
    /// The sparse map between the independent space and this space
    sparse_map_register m_sm_;
};

using CanonicalIndSpace = IndependentSpace<CanonicalSpaceD>;

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
template<typename LHS, typename RHS>
bool operator==(const IndependentSpace<LHS>& lhs,
                const IndependentSpace<RHS>& rhs) {
    if(lhs.sparse_map() != rhs.sparse_map()) return false;
    const LHS& plhs = lhs;
    const RHS& prhs = rhs;
    return lhs == rhs;
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
template<typename LHS, typename RHS>
bool operator!=(const IndependentSpace<LHS>& lhs,
                const IndependentSpace<RHS>& rhs) {
    return !(lhs == rhs);
}

// -------------------------------- Implementations ----------------------------

template<typename BaseType>
IndependentSpace<BaseType>::IndependentSpace(BaseType base,
                                             sparse_map_register sm) :
  m_sm_(std::move(sm)), BaseType(std::move(base)) {}

template<typename BaseType>
template<typename DepSpace>
std::string IndependentSpace<BaseType>::key_(const DepSpace& dep) {
    return pluginplay::hash_objects(dep);
}

template<typename BaseType>
inline bool IndependentSpace<BaseType>::equal_(
  const BaseSpace& rhs) const noexcept {
    return equal_common(*this, rhs);
}

} // namespace libchemist::orbital_space
