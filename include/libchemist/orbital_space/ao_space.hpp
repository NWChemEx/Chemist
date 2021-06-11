#pragma once
#include "libchemist/basis_set/basis_set.hpp"
#include "libchemist/orbital_space/base_space.hpp"
#include "libchemist/orbital_space/dependent_space.hpp"

namespace libchemist::orbital_space {

/** @brief Models a space spanned by atomic orbitals.
 *
 *  The AO space takes on a special role in quantum chemistry in that it is a
 *  sort of lowest common denominator among spaces. This is because we usually
 *  know how to compute integrals in the AO space, but not directly in the
 *  derived spaces.
 *
 *  @tparam BasisType The type of the object holding the basis set parameters.
 *  @tparam BaseType  The type of the class this class derives from. It is
 *                    assumed to be either BaseSpace or DependentSpace.
 */
template<typename BasisType, typename BaseType>
class AOSpace : public BaseType {
private:
    /// Type of this instance
    using my_type = AOSpace<BasisType, BaseType>;

public:
    /// Type of the object holding the AO basis set parameters
    using basis_type = BasisType;

    /// Type used for indexing and offsets
    using size_type = typename BaseType::size_type;

    /** @brief Creates a new AOSpace. The AOSpace has no AO basis set and the
     *         base class is also default initialized.
     *
     *
     */
    AOSpace() = default;

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
    explicit AOSpace(basis_type bs, Args&&... args);

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
    virtual size_type size_() const noexcept override;

    virtual bool equal_(const BaseSpace& rhs) const noexcept override;

private:
    /// The object holding the basis set parameters
    basis_type m_bs_;
};

template<typename LHSAO, typename LHSBase, typename RHSAO, typename RHSBase>
inline bool operator==(const AOSpace<LHSAO, LHSBase>& lhs,
                       const AOSpace<RHSAO, RHSBase>& rhs) {
    // Must be same type
    if constexpr(!std::is_same_v<LHSAO, RHSAO>)
        return false;
    else if constexpr(!std::is_same_v<LHSBase, RHSBase>)
        return false;

    // Compare the basis sets
    if(lhs.basis_set() != rhs.basis_set()) return false;

    // If derived directly from BaseSpace no BaseType::operator== to call
    if constexpr(std::is_same_v<LHSBase, BaseSpace>)
        return true;
    else { // call operator== for BaseType
        const LHSBase& lbase = lhs;
        const RHSBase& rbase = rhs;
        return lhs == rhs;
    }
}

template<typename LHSAO, typename LHSBase, typename RHSAO, typename RHSBase>
inline bool operator!=(const AOSpace<LHSAO, LHSBase>& lhs,
                       const AOSpace<RHSAO, RHSBase>& rhs) {
    return !(lhs == rhs);
}

using AOSpaceD          = AOSpace<AOBasisSetD, BaseSpace>;
using AOSpaceF          = AOSpace<AOBasisSetF, BaseSpace>;
using DependentAOSpaceD = AOSpace<AOBasisSetD, DependentSpace>;
using DependentAOSpaceF = AOSpace<AOBasisSetF, DependentSpace>;

// ---------------------------- Implementations -------------------------------

template<typename BasisType, typename BaseType>
template<typename... Args>
AOSpace<BasisType, BaseType>::AOSpace(basis_type bs, Args&&... args) :
  BaseType(std::forward<Args>(args)...), m_bs_(std::move(bs)) {}

template<typename BasisType, typename BaseType>
void AOSpace<BasisType, BaseType>::hash_(sde::Hasher& h) const {
    if constexpr(std::is_same_v<BaseType, DerivedSpace>) { BaseType::hash_(h); }
    h(m_bs_);
}

template<typename BasisType, typename BaseType>
void AOSpace<BasisType, BaseType>::size_(sde::Hasher& h) const noexcept {
    if constexpr(std::is_same_v<BaseType, BaseSpace>) {
        return m_bs_.size();
    } else {
        return BaseType::size_();
    }
}

template<typename BasisType, typename BaseType>
bool AOSpace<BasisType, BaseType>::equal_(const BaseSpace& rhs) const noexcept {
    return this->equal_common(*this, rhs);
}

} // namespace libchemist::orbital_space
