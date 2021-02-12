#pragma once
#include <sde/detail_/memoization.hpp>

namespace libchemist::orbital_space {

/** @brief Models a space spanned by atomic orbitals.
 *
 *  The AO space takes on a special role in quantum chemistry in that it is a
 *  sort of lowest common denominator among spaces. This is because we usually
 *  know how to compute integrals in the AO space, but not directly in the
 *  derived spaces.
 *
 *  In addition to an overlap matrix the AO space also contains an instance of
 *  the parameters which define the atomic orbitals.
 *
 *  @tparam BasisType The type of the object holding the basis set parameters.
 *  @tparam BaseType  The type of the class this class derives from. It
 *                    minimally must define a virtual `hash_` method.
 */
template<typename BasisType, typename BaseType>
class AOSpace_ : public BaseType {
private:
    /// Type of the base class
    using base_type = BaseType;

public:
    /// Type of the object holding the AO basis set parameters
    using basis_type = BasisType;

    /// Type used for indexing and offsets
    using size_type = typename BaseType::size_type;

    /** @brief Creates a new AOSpace. The AOSpace has no AO basis set or overlap
     *         matrix.
     *
     *
     */
    AOSpace_() = default;

    /** @brief Creates a new atomic orbital space.
     *
     * @tparam Args The types of the arguments which will be forwarded to the
     *              base class's ctor. These types will be deduced automatically
     *              and the user should not have to specify them.
     *
     * @param[in] bs The parameters of the atomic orbitals.
     * @param[in] args Inputs to forward to the base class's ctor.
     */
    template<typename... Args>
    AOSpace_(basis_type bs, Args&&... args);

    /** @brief Accessor for read/write access to the basis set parameters
     *
     *  @return The basis set parameters in a read/write state.
     */
    auto& basis_set() { return m_bs_; }

    /** @brief Accessor for read-only access to the basis set parameters
     *
     *  @return The basis set parameters in a read-only state.
     */
    const auto& basis_set() const { return m_bs_; }

protected:
    /** @brief Overrides hashing to account for the basis set
     *
     *  @param[in,out] h The hasher instance being used. After this call the
     *                   AO basis set wil be added to the internal hashed state.
     */
    virtual void hash_(sde::Hasher& h) const override;

    /** @brief Overrides the size member so that it returns the number of AOs.
     *
     *  This function simply calls the `size()` member of the AO basis set and
     *  returns the result.
     */
    virtual size_type size_() const noexcept override { return m_bs_.size(); }

private:
    /// The object holding the basis set parameters
    basis_type m_bs_;
};

template<typename BasisType, typename LHSBase, typename OtherType>
bool operator==(const AOSpace_<BasisType, LHSBase>& lhs, OtherType&& rhs) {
    using clean_lhs_t = std::decay_t<decltype(lhs)>;
    using clean_rhs_t = std::decay_t<decltype(rhs)>;
    if constexpr(std::is_same_v<clean_lhs_t, clean_rhs_t>) {
        const auto& cast_lhs = static_cast<const LHSBase&>(lhs);
        const auto& cast_rhs = static_cast<const LHSBase&>(rhs);
        return (cast_lhs == cast_rhs) && (lhs.basis_set() == rhs.basis_set());
    } else {
        return false;
    }
}

template<typename BasisType, typename LHSBase, typename OtherType>
bool operator!=(const AOSpace_<BasisType, LHSBase>& lhs, OtherType&& rhs) {
    return !(lhs == std::forward<OtherType>(rhs));
}

// ---------------------------- Implementations -------------------------------

template<typename BasisType, typename BaseType>
template<typename... Args>
AOSpace_<BasisType, BaseType>::AOSpace_(basis_type bs, Args&&... args) :
  BaseType(std::forward<Args>(args)...), m_bs_(std::move(bs)) {}

template<typename BasisType, typename BaseType>
void AOSpace_<BasisType, BaseType>::hash_(sde::Hasher& h) const {
    BaseType::hash_(h);
    h(m_bs_);
}

} // namespace libchemist::orbital_space
