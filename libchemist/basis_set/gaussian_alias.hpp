#pragma once
#include "libchemist/basis_set/detail_/utility.hpp"
#include "libchemist/basis_set/gaussian.hpp"

namespace libchemist::basis_set {

template<typename BasisSetType, typename IndexType>
class GaussianAlias {
private:
    using clean_bs = std::decay_t<std::remove_pointer_t<BasisSetType>>;

public:
    /// The floating-point type used to hold the parameters
    using value_type = typename clean_bs::value_type;

    explicit constexpr GaussianAlias(BasisSetType bs, IndexType idx) noexcept;

    constexpr auto& coefficient() noexcept;
    constexpr auto& coefficient() const noexcept;

    constexpr auto& exponent() noexcept;
    constexpr auto& exponent() const noexcept;

    constexpr bool has_center() const noexcept;
    constexpr auto& center() noexcept;
    constexpr auto& center() const noexcept;

private:
    IndexType m_index_;
    detail_::holder<BasisSetType> m_bs_;
};

template<typename LBS, typename LIDX, typename RHS>
constexpr bool operator==(const GaussianAlias<LBS, LIDX>& lhs,
                          const RHS& rhs) noexcept;

template<typename LBS, typename LIDX, typename RHS>
constexpr bool operator!=(const GaussianAlias<LBS, LIDX>& lhs,
                          const RHS& rhs) noexcept;

//-------------------------------Implementations--------------------------------
#define VIEW_TYPE GaussianAlias<BasisSetType, IndexType>

template<typename BasisSetType, typename IndexType>
constexpr VIEW_TYPE::GaussianAlias(BasisSetType bs, IndexType idx) noexcept :
  m_index_(std::move(idx)),
  m_bs_(bs) {}

template<typename BasisSetType, typename IndexType>
constexpr auto& VIEW_TYPE::coefficient() noexcept {
    return detail_::CallMemberX::coefficient(m_bs_, m_index_);
}

template<typename BasisSetType, typename IndexType>
constexpr auto& VIEW_TYPE::coefficient() const noexcept {
    return detail_::CallMemberX::coefficient(m_bs_, m_index_);
}

template<typename BasisSetType, typename IndexType>
constexpr auto& VIEW_TYPE::exponent() noexcept {
    return detail_::CallMemberX::exponent(m_bs_, m_index_);
}

template<typename BasisSetType, typename IndexType>
constexpr auto& VIEW_TYPE::exponent() const noexcept {
    return detail_::CallMemberX::exponent(m_bs_, m_index_);
}

template<typename BasisSetType, typename IndexType>
constexpr bool VIEW_TYPE::has_center() const noexcept {
    return detail_::CallMemberX::has_center(m_bs_, m_index_);
}

template<typename BasisSetType, typename IndexType>
constexpr auto& VIEW_TYPE::center() noexcept {
    return detail_::CallMemberX::center(m_bs_, m_index_);
}

template<typename BasisSetType, typename IndexType>
constexpr auto& VIEW_TYPE::center() const noexcept {
    return detail_::CallMemberX::center(m_bs_, m_index_);
}

template<typename LBS, typename LIDX, typename RHS>
constexpr bool operator==(const GaussianAlias<LBS, LIDX>& lhs,
                          const RHS& rhs) noexcept {
    return detail_::compare_gaussians(lhs, rhs);
}

template<typename LBS, typename LIDX, typename RHS>
constexpr bool operator!=(const GaussianAlias<LBS, LIDX>& lhs,
                          const RHS& rhs) noexcept {
    return !(lhs == rhs);
}

#undef VIEW_TYPE
} // namespace libchemist::basis_set
