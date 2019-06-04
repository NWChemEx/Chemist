#pragma once
#include "libchemist/basis_set/gaussian_ao_alias.hpp"

namespace libchemist::basis_set {

template<typename T, typename Container = std::vector<T>>
class GaussianShell {
public:
    template<typename CoefType, typename ExpType, typename CoordType>
    constexpr GaussianShell(bool pure, type::angular_momentum l, CoefType&& cs,
                            ExpType&& as, CoordType&& r) noexcept;

    constexpr auto ao(type::size i) noexcept;

    constexpr auto ao(type::size i) const noexcept;

    constexpr auto nao() const noexcept;

    constexpr auto& pure() noexcept { return m_is_pure_; }

    constexpr auto& pure() const noexcept { return m_is_pure_; }

    constexpr auto& l() noexcept { return m_l_; }

    constexpr auto& l() const noexcept { return m_l_; }

    constexpr auto& coefficient(type::size i) noexcept { return m_coefs_[i]; }

    constexpr auto& coefficient(type::size i) const noexcept;

    constexpr auto& exponent(type::size i) noexcept { return m_alpha_[i]; }
    constexpr auto& exponent(type::size i) const noexcept;

    template<typename... Idxs>
    constexpr bool has_center(Idxs...) const noexcept;

    constexpr auto& center() noexcept { return m_coord_.value(); }

    constexpr auto& center() const noexcept { return m_coord_.value(); }

private:
    /// Allows views to get at data
    friend class detail_::CallMemberX;

    /// Binomial coefficient @p m choose @p n
    constexpr auto bc(type::size m, type::size n) const noexcept;

    /// The number of primitives in AO @p i
    constexpr auto nprims(type::size) const noexcept { return m_coefs_.size(); }

    /// The @p j-th primitive in AO @p i
    constexpr auto primitive(type::size i, type::size j) const noexcept;

    /// The @p j-th contraction coefficient in AO @p i
    constexpr auto& coefficient(type::size, type::size j) const noexcept;

    /// The @p j-th exponent in AO @p i
    constexpr auto& exponent(type::size, type::size j) const noexcept;

    /// The center in a read-only state
    template<typename... Idxs>
    constexpr auto& center(type::size, Idxs...) const;

    /// Is this a spherical shell?
    bool m_is_pure_;

    /// The magnitude of the angular momentum
    type::angular_momentum m_l_;

    /// The contraction coefficients for the primitives
    Container m_coefs_;

    /// The exponents for the primitives
    Container m_alpha_;

    ///(possibly) the Cartesian coordinates of the shell, in atomic units
    std::optional<type::coord<T>> m_coord_;
};

template<typename CoefContainer, typename ExpContainer>
constexpr auto make_gaussian_shell(bool pure, type::angular_momentum l,
                                   CoefContainer&& cs,
                                   ExpContainer&& as) noexcept;

template<typename CoefContainer, typename ExpContainer, typename CoordType>
constexpr auto make_gaussian_shell(bool pure, type::angular_momentum l,
                                   CoefContainer&& cs, ExpContainer&& as,
                                   CoordType&& r) noexcept;

template<typename LHST, typename LHSCont, typename RHS>
constexpr bool operator==(const GaussianShell<LHST, LHSCont>& lhs,
                          RHS&& rhs) noexcept;

template<typename LHST, typename LHSCont, typename RHS>
constexpr bool operator!=(const GaussianShell<LHST, LHSCont>& lhs,
                          RHS&& rhs) noexcept;

//------------------------------Implementations---------------------------------

#define CLASS_PARAMS template<typename T, typename Container>
#define CLASS_TYPE GaussianShell<T, Container>

CLASS_PARAMS
template<typename CoefType, typename ExpType, typename CoordType>
constexpr CLASS_TYPE::GaussianShell(bool pure, type::angular_momentum l,
                                    CoefType&& cs, ExpType&& as,
                                    CoordType&& r) noexcept :
  m_is_pure_(pure),
  m_l_(l),
  m_coefs_(std::forward<CoefType>(cs)),
  m_alpha_(std::forward<ExpType>(as)),
  m_coord_(std::forward<CoordType>(r)) {}

CLASS_PARAMS
constexpr auto CLASS_TYPE::ao(type::size i) noexcept {
    return GaussianAOAlias{this, std::array{i}};
}

CLASS_PARAMS
constexpr auto CLASS_TYPE::ao(type::size i) const noexcept {
    return GaussianAOAlias{this, std::array{i}};
}

CLASS_PARAMS
constexpr auto CLASS_TYPE::nao() const noexcept {
    return m_is_pure_ ? 2 * m_l_ + 1 : bc(m_l_ + 2, 2);
}

CLASS_PARAMS
constexpr auto& CLASS_TYPE::coefficient(type::size i) const noexcept {
    return m_coefs_[i];
}

CLASS_PARAMS
constexpr auto& CLASS_TYPE::exponent(type::size i) const noexcept {
    return m_alpha_[i];
}

CLASS_PARAMS
template<typename... Idxs>
constexpr bool CLASS_TYPE::has_center(Idxs...) const noexcept {
    return m_coord_.has_value();
}

CLASS_PARAMS
constexpr auto CLASS_TYPE::bc(type::size m, type::size n) const noexcept {
    if(n == 0 || n == m) return 1;
    return bc(m - 1, n - 1) + bc(m - 1, n);
}

CLASS_PARAMS
constexpr auto CLASS_TYPE::primitive(type::size i, type::size j) const
  noexcept {
    return GaussianAlias{this, std::array{i, j}};
}

CLASS_PARAMS
constexpr auto& CLASS_TYPE::coefficient(type::size, type::size j) const
  noexcept {
    return m_coefs_[j];
}

CLASS_PARAMS
constexpr auto& CLASS_TYPE::exponent(type::size, type::size j) const noexcept {
    return m_alpha_[j];
}

CLASS_PARAMS
template<typename... Idxs>
constexpr auto& CLASS_TYPE::center(type::size, Idxs...) const {
    return m_coord_.value();
}

template<typename CoefContainer, typename ExpContainer>
constexpr auto make_gaussian_shell(bool pure, type::angular_momentum l,
                                   CoefContainer&& cs,
                                   ExpContainer&& as) noexcept {
    return make_gaussian_shell(pure, l, std::forward<CoefContainer>(cs),
                               std::forward<ExpContainer>(as), std::nullopt);
}

template<typename CoefContainer, typename ExpContainer, typename CoordType>
constexpr auto make_gaussian_shell(bool pure, type::angular_momentum l,
                                   CoefContainer&& cs, ExpContainer&& as,
                                   CoordType&& r) noexcept {
    using container_type = std::decay_t<CoefContainer>;
    static_assert(std::is_same_v<container_type, std::decay_t<ExpContainer>>,
                  "Coefficients and exponents must have same container type");
    using value_type = typename container_type::value_type;
    using shell_type = GaussianShell<value_type, container_type>;
    return shell_type{pure, l, std::forward<CoefContainer>(cs),
                      std::forward<ExpContainer>(as),
                      std::forward<CoordType>(r)};
}

template<typename LHST, typename LHSCont, typename RHS>
constexpr bool operator==(const GaussianShell<LHST, LHSCont>& lhs,
                          RHS&& rhs) noexcept {
    return detail_::compare_shell(lhs, rhs);
}

template<typename LHST, typename LHSCont, typename RHS>
constexpr bool operator!=(const GaussianShell<LHST, LHSCont>& lhs,
                          RHS&& rhs) noexcept {
    return !(lhs == rhs);
}

#undef CLASS_TYPE
#undef CLASS_PARAMS
} // namespace libchemist::basis_set
