#pragma once
#include "libchemist/basis_set/gaussian_shell_alias.hpp"

namespace libchemist::basis_set {

/** @brief A collection of Gaussian shells on the same center
 *
 * @tparam PrimCont The type of th
 * @tparam ShellCont
 */
template<typename PrimCont, typename ShellCont>
class GaussianCenter {
private:
    using my_type = GaussianCenter<PrimCont, ShellCont>;

public:
    using value_type = typename PrimCont::value_type;

    constexpr GaussianCenter() = default;

    template<typename ShellType, typename... Args>
    explicit constexpr GaussianCenter(ShellType&& s0, Args&&... args);

    constexpr type::size nshells() const noexcept { return m_nprims_.size(); }

    constexpr auto shell(type::size i);
    constexpr auto shell(type::size i) const;

    template<typename... Idxs>
    constexpr bool has_center(Idxs&&...) const noexcept;

    template<typename CoordType>
    constexpr void set_center(CoordType&& r) noexcept;

    constexpr auto& center() { return m_coord_.value(); }
    constexpr auto& center() const { return m_coord_.value(); }

private:
    /// Allows views to work
    friend class detail_::CallMemberX;

    ///
    constexpr auto& pure(type::size i) { return m_pure_[i]; }
    constexpr auto& pure(type::size i) const { return m_pure_[i]; }

    constexpr auto& l(type::size i) { return m_l_[i]; }
    constexpr auto& l(type::size i) const { return m_l_[i]; }

    constexpr auto naos(type::size i) const {
        const auto li = m_l_[i];
        return m_pure_[i] ? 2 * li + 1 : detail_::bc(li + 2, 2);
    }

    constexpr auto ao(type::size i, type::size j) const {
        return GaussianAOAlias{this, std::array{i, j}};
    }

    constexpr auto nprims(type::size i, type::size) const {
        return m_nprims_[i];
    }

    constexpr auto primitive(type::size i, type::size j, type::size k) const {
        return GaussianAlias{this, std::array{i, j, k}};
    }

    constexpr auto index_(type::size i, type::size, type::size k) const {
        return (i != 0 ? m_offsets_[i - 1] : 0) + k;
    }

    constexpr auto& coefficient(type::size i, type::size j,
                                type::size k) const {
        return m_coefs_[index_(i, j, k)];
    }

    constexpr auto& exponent(type::size i, type::size j, type::size k) const {
        return m_alpha_[index_(i, j, k)];
    }

    template<typename... Idxs>
    constexpr auto& center(type::size, Idxs&&...) const {
        return m_coord_.value();
    }

    template<type::size depth, bool off, typename T, typename ShellType,
             typename... Args>
    static constexpr auto loop_shell_(T&& rv, const ShellType& s,
                                      const Args&... args);

    /// Extracts the number of primitives or the offsets form a series of shells
    template<bool off, typename ShellType, typename... Args>
    static constexpr auto get_shell_(const ShellType& s, const Args&... args);

    template<type::size depth, bool off, typename T, typename ShellType,
             typename... Args>
    static constexpr auto loop_param_(type::size& counter, T&& rv,
                                      const ShellType& s, const Args&... args);

    /// Extracts the coefficients or exponents from a series of shells
    template<bool coef, typename ShellType, typename... Args>
    static constexpr auto get_param_(const ShellType& s, const Args&... args);

    /// The number of primitives in each shell
    ShellCont m_nprims_;

    /// The @p i-th element is the start of shell i+1
    ShellCont m_offsets_;

    /// The purity of each shell
    ShellCont m_pure_;

    /// The angular_moment of each shell
    ShellCont m_l_;

    /// The contraction coefficients concatenated by shell
    PrimCont m_coefs_;

    /// The exponents concatenated by shell
    PrimCont m_alpha_;

    ///(possibly) where this collection of shells is centered
    std::optional<type::coord<value_type>> m_coord_;
};

template<typename LHS1, typename LHS2, typename RHS>
constexpr bool operator==(const GaussianCenter<LHS1, LHS2>& lhs,
                          RHS&& rhs) noexcept;

template<typename LHS1, typename LHS2, typename RHS>
constexpr bool operator!=(const GaussianCenter<LHS1, LHS2>& lhs,
                          RHS&& rhs) noexcept;

//------------------------Implementations---------------------------------------
#define CLASS_PARAMS template<typename PrimCont, typename ShellCont>
#define CLASS_TYPE GaussianCenter<PrimCont, ShellCont>
#define SHELL_PARAMS template<typename ShellType, typename... Args>

GaussianCenter()->GaussianCenter<std::vector<double>, std::vector<double>>;

SHELL_PARAMS
explicit GaussianCenter(ShellType&&, Args&&...)
  ->GaussianCenter<detail_::gaussian_center_container_t<std::decay_t<ShellType>,
                                                        std::decay_t<Args>...>,
                   std::array<type::size, 1 + sizeof...(Args)>>;

CLASS_PARAMS
SHELL_PARAMS
constexpr CLASS_TYPE::GaussianCenter(ShellType&& s0, Args&&... args) :
  m_nprims_(my_type::get_shell_<false>(s0, args...)),
  m_offsets_(my_type::get_shell_<true>(s0, args...)),
  m_pure_({s0.pure(), args.pure()...}),
  m_l_({s0.l(), args.l()...}),
  m_coefs_(my_type::get_param_<true>(s0, args...)),
  m_alpha_(my_type::get_param_<false>(s0, args...)),
  m_coord_(s0.has_center() ? std::optional{s0.center()} : std::nullopt) {}

CLASS_PARAMS
constexpr auto CLASS_TYPE::shell(type::size i) {
    return GaussianShellAlias{this, std::array{i}};
}

CLASS_PARAMS
constexpr auto CLASS_TYPE::shell(type::size i) const {
    return GaussianShellAlias{this, std::array{i}};
}

CLASS_PARAMS
template<typename... Idxs>
constexpr bool CLASS_TYPE::has_center(Idxs&&...) const noexcept {
    return m_coord_.has_value();
}

CLASS_PARAMS
template<typename CoordType>
constexpr void CLASS_TYPE::set_center(CoordType&& r) noexcept {
    m_coord_.emplace(std::forward<CoordType>(r));
}

CLASS_PARAMS
template<type::size depth, bool off, typename T, typename ShellType,
         typename... Args>
constexpr auto CLASS_TYPE::loop_shell_(T&& rv, const ShellType& s,
                                       const Args&... args) {
    constexpr bool is_array = detail_::is_array_v<std::decay_t<T>>;
    const auto& shell_i     = std::get<depth>(std::tie(s, args...));
    const auto nprim        = shell_i.ao(0).nprims();
    const auto offi         = depth == 0 ? 0 : rv[depth - 1];
    const auto ci           = off ? offi + nprim : nprim;
    if constexpr(is_array)
        rv[depth] = ci;
    else
        rv.push_back(ci);
    if constexpr(depth == sizeof...(Args))
        return std::forward<T>(rv);
    else
        return loop_shell_<depth + 1, off>(std::forward<T>(rv), s, args...);
}

CLASS_PARAMS
template<bool off, typename ShellType, typename... Args>
constexpr auto CLASS_TYPE::get_shell_(const ShellType& s, const Args&... args) {
    ShellCont params{};
    return loop_shell_<0, off>(std::move(params), s, args...);
}

CLASS_PARAMS
template<type::size depth, bool coef, typename T, typename ShellType,
         typename... Args>
constexpr auto CLASS_TYPE::loop_param_(type::size& counter, T&& rv,
                                       const ShellType& s,
                                       const Args&... args) {
    constexpr bool is_array = detail_::is_array_v<std::decay_t<T>>;
    const auto& shell_i     = std::get<depth>(std::tie(s, args...));
    for(type::size prim_i = 0; prim_i < shell_i.ao(0).nprims(); ++prim_i) {
        const auto ci = coef ? shell_i.ao(0).primitive(prim_i).coefficient() :
                               shell_i.ao(0).primitive(prim_i).exponent();
        if constexpr(is_array)
            rv[counter] = ci;
        else
            rv.push_back(ci);
        ++counter;
    }
    if constexpr(depth == sizeof...(Args))
        return std::forward<T>(rv);
    else
        return loop_param_<depth + 1, coef>(counter, std::forward<T>(rv), s,
                                            args...);
}

CLASS_PARAMS
template<bool coef, typename ShellType, typename... Args>
constexpr auto CLASS_TYPE::get_param_(const ShellType& s, const Args&... args) {
    PrimCont rv{};
    type::size counter = 0;
    return loop_param_<0, coef>(counter, std::move(rv), s, args...);
}

template<typename LHS1, typename LHS2, typename RHS>
constexpr bool operator==(const GaussianCenter<LHS1, LHS2>& lhs,
                          RHS&& rhs) noexcept {
    return detail_::compare_center(lhs, std::forward<RHS>(rhs));
}

template<typename LHS1, typename LHS2, typename RHS>
constexpr bool operator!=(const GaussianCenter<LHS1, LHS2>& lhs,
                          RHS&& rhs) noexcept {
    return !(lhs == rhs);
}

#undef SHELL_PARAMS
#undef CLASS_TYPE
#undef CLASS_PARAMS

} // namespace libchemist::basis_set
