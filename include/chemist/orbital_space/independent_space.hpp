#pragma once
#include "chemist/orbital_space/canonical_space.hpp"
#include "chemist/orbital_space/type_traits/is_independent.hpp"
#include "chemist/types.hpp"

namespace chemist::orbital_space {

/** @brief An orbital space which serves as an independent space for one or
 *         more sparse maps.
 *
 *  Given a sparse map relation between from one space to another space, we term
 *  the "from space" the "independent space" and the "to space" is termed the
 *  "dependent space". This class wraps an independent space and associates with
 *  it a series of dependent spaces.
 *
 *  @note SparseMaps can be inverted which means that the distinction of
 *        independent/dependent is problem-specific.
 *
 *  @note To store relations this class hashes the dependent space. If the
 *        dependent space is actually an `IndependentSpace<T>` instance this
 *        class will only hash the `T` part. This is to avoid the fact that the
 *        hash of the full class would change when sparse maps are added, even
 *        though the orbitals remain the same. Put another way if two
 *        `IndependentSpace<T>` instances differ only in the sparse maps they
 *        contain, this class can not tell them apart.
 */
template<typename BaseType>
class IndependentSpace : public BaseType {
public:
    /// Type of the base space
    using base_space_type = BaseType;

    /// The type used for indexing and offsets
    using size_type = typename BaseType::size_type;

    /// The type of the sparse map stored in this instance
    using sparse_map_type = type::sparse_map;

    /// How the sparse maps are differentiated
    using sparse_map_register = std::map<std::string, sparse_map_type>;

    /** @brief Creates a defaulted IndependentSpace instance.
     *
     *  The instance resulting from this call has no state and can only gain
     *  state by being assigned/moved to.
     *
     *  @throw None No throw guarantee.
     */
    IndependentSpace() = default;

    /** @brief Creates a new IndependentSpace by associating the provided space
     *         with a set of sparse maps.
     *
     *  This ctor can be used to create a new IndependentSpace instance which
     *  associates @p base with the sparse maps @p sms. Additional sparse maps
     *  can be added by calling `add_sparse_map`.
     *
     *  @param[in] base The orbital space which is an independent space in one
     *                  or more sparse map relations.
     *  @param[in] sms The sparse maps to associate with @p base. Default value
     *                 is an empty map.
     *
     *  @throw None No throw guarantee.
     */
    explicit IndependentSpace(BaseType base, sparse_map_register sms = {});

    /** @brief Registers a sparsity relation between this orbital space and
     *         @p dep.
     *
     *  This function registers a sparse map `L(i->a)`, where `i` is a member of
     *  the current space and `a` is a member of the orbital space described by
     *  @p dep. @p dep is allowed to be `*this`.
     *
     *  @tparam DepSpace The type of @p dep. Assumed to derive from `BaseSpace`.
     *
     *  @param[in] dep The dependent space in the sparse map relation (the
     *                 independent space is this space).
     *  @param[in] sm The sparse map defining the relation between this space
     *                and @p dep.
     *
     *  @throw std::bad_alloc if there is a problem storing the relation. Strong
     *                        throw guarantee.
     */
    template<typename DepSpace>
    void add_sparse_map(const DepSpace& dep, sparse_map_type sm);

    /** @brief Returns the number of sparse relations between this space and
     *         @p dep.
     *
     *  This function is used to determine how many stored sparse maps describe
     *  relationships with the current space as the independent space and @p dep
     *  as the dependent space. The result is either 0 or 1 because there can
     *  not be multiple sparse map relationships between the same two spaces.
     *
     *  @tparam DepSpace The type of the dependent space in the relationship.
     *                   Assumed to derive from BaseSpace.
     *
     *  @param[in] dep The dependent space in the sparsity relationship of
     *                 interest (`*this` is the independent space).
     *
     *  @return 0 if there is no registered sparsity relationship between this
     *          instance and @p dep. 1 if there is.
     *
     *  @throw ??? If an exceptions is raised while hashing. Same exception
     *             guarantee as the hash function.
     */
    template<typename DepSpace>
    auto count(const DepSpace& dep) const {
        return m_sm_.count(key_(dep));
    }

    /** @brief The sparse map from this space to the requested space
     *
     *  @return The internal SparseMap instance in a read-only format.
     *
     *  @throw std::out_of_range if there is no map associated with @p dep.
     *                           Strong throw guarantee.
     */
    template<typename DepSpace>
    const auto& sparse_map(const DepSpace& dep) const {
        return m_sm_.at(key_(dep));
    }

    /** @brief Determines if this orbital space is the same as @p rhs.
     *
     *  This function determines if the current instance is the same as @p rhs.
     *  Equality entails having the same set of sparse maps and having base
     *  class instances which compare equal.
     *
     *  @return If the current instance is equivalent to @p rhs and false
     *          otherwise.
     *
     *  @throw None No throw guarantee.
     */
    bool operator==(const IndependentSpace& rhs) const;

protected:
    /// Adds the hash of the sparse map to the provided hasher.
    virtual void hash_(chemist::detail_::Hasher& h) const override;

    /// Returnst true if the spaces have the same sparse map
    virtual bool equal_(const BaseSpace& rhs) const noexcept override;

private:
    /// Allows other instances to use base_hash_();
    template<typename OtherBase>
    friend class IndependentSpace;

    /// Wraps the process of only hashing the base state
    std::string base_hash_() const;

    /** @brief Wraps the process of converting a space into a hash.
     *
     *  If @p DepSpace is an instance of `IndependentSpace<T>` this function
     *  will only hash the `T` part; otherwise this function hashes all of @p
     *  dep.
     *
     *  @tparam DepSpace The type of the space we are hashing. Assumed to derive
     *                   from BaseSpace.
     *
     *  @param[in] dep The space we are hashing.
     *
     *  @return The hash of @p dep.
     *
     *  @throw ??? If the hash function throws. Same throw guarantee.
     */
    template<typename DepSpace>
    static std::string key_(const DepSpace& dep);

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
    if constexpr(!std::is_same_v<decltype(lhs), decltype(rhs)>)
        return false;
    else {
        return lhs.operator==(rhs);
    }
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
void IndependentSpace<BaseType>::add_sparse_map(const DepSpace& dep,
                                                sparse_map_type sm) {
    m_sm_.emplace(key_(dep), std::move(sm));
}

template<typename BaseType>
bool IndependentSpace<BaseType>::operator==(const IndependentSpace& rhs) const {
    if(m_sm_ != rhs.m_sm_) return false;
    using const_ref = const BaseType&;
    return static_cast<const_ref>(*this) == static_cast<const_ref>(rhs);
}

template<typename BaseType>
std::string IndependentSpace<BaseType>::base_hash_() const {
    auto h = chemist::detail_::make_hasher();
    BaseType::hash_(h);
    return chemist::detail_::hash_to_string(h.finalize());
}

template<typename BaseType>
template<typename DepSpace>
std::string IndependentSpace<BaseType>::key_(const DepSpace& dep) {
    if constexpr(is_independent_space_v<DepSpace>) {
        return dep.base_hash_();
    } else {
        return chemist::detail_::hash_objects(dep);
    }
}

template<typename BaseType>
inline bool IndependentSpace<BaseType>::equal_(
  const BaseSpace& rhs) const noexcept {
    return this->equal_common(*this, rhs);
}

template<typename BaseType>
void IndependentSpace<BaseType>::hash_(chemist::detail_::Hasher& h) const {
    BaseType::hash_(h);
    h(m_sm_);
}

} // namespace chemist::orbital_space
