#pragma once
#include <optional>
#include <tuple>

namespace libchemist::basis_set {
// Forward declare basis set classes for template meta-programming purposes
template<typename T>
class Gaussian;
template<typename T, typename U>
class GaussianAlias;

namespace detail_ {

template<typename T>
using holder = std::conditional_t<std::is_pointer_v<T>, T, std::optional<T>>;

template<typename T, typename U>
constexpr bool compare_gaussians(T&& lhs, U&& rhs) noexcept {
    // Check the coefficients and exponents
    const auto lhs1 = std::tie(lhs.coefficient(), lhs.exponent());
    const auto rhs1 = std::tie(rhs.coefficient(), rhs.exponent());
    if(lhs1 != rhs1) return false;

    // Check the center-having-ness
    const bool lcenter = lhs.has_center();
    const bool rcenter = rhs.has_center();
    if((!lcenter) && (!rcenter))
        return true; // Both don't have centers
    else if(lcenter != rcenter)
        return false; // One has center other doesn't

    // Check the centers' actual values
    const auto& lr = lhs.center();
    const auto& rr = rhs.center();
    return std::tie(lr[0], lr[1], lr[2]) == std::tie(rr[0], rr[1], rr[2]);
}

template<typename T, typename U>
constexpr bool compare_ao(T&& lhs, U&& rhs) noexcept {
    if(lhs.nprims() != rhs.nprims()) return false;
    for(std::size_t i = 0; i < lhs.nprims(); ++i)
        if(lhs.primitive(i) != rhs.primitive(i)) return false;
    return true;
}

template<typename T, typename U>
constexpr bool compare_shell(T&& lhs, U&& rhs) noexcept {
    const auto lparams = std::tie(lhs.pure(), lhs.l());
    const auto rparams = std::tie(rhs.pure(), rhs.l());
    if(lparams != rparams) return false;
    if(lhs.nao() != rhs.nao()) return false;
    for(std::size_t i = 0; i < lhs.nao(); ++i)
        if(lhs.ao(i) != rhs.ao(i)) return false;
    return true;
}

template<typename Idx, typename... Idxs>
constexpr auto cat_indices(Idx&& lhs, Idxs... idxs) noexcept {
    constexpr auto N = std::tuple_size_v<std::remove_reference_t<Idx>>;
    constexpr auto M = N + sizeof...(Idxs);
    return std::tuple_cat(lhs, std::array{idxs...});
}

#define CALL_MEMBER_X(fxn_name, rv)                                         \
    template<typename BS, typename Idx, std::size_t... Is>                  \
    static constexpr rv fxn_name(BS&& bs, Idx&& idx,                        \
                                 std::index_sequence<Is...>) {              \
        return (*bs).fxn_name(std::get<Is>(idx)...);                        \
    }                                                                       \
    template<typename BS, typename Idx>                                     \
    static constexpr rv fxn_name(BS&& bs, Idx&& idx) {                      \
        constexpr auto N = std::tuple_size_v<std::remove_reference_t<Idx>>; \
        return fxn_name(std::forward<BS>(bs), std::forward<Idx>(idx),       \
                        std::make_index_sequence<N>{});                     \
    }

struct CallMemberX {
    CALL_MEMBER_X(coefficient, auto&)
    CALL_MEMBER_X(exponent, auto&)
    CALL_MEMBER_X(center, auto&)
    CALL_MEMBER_X(has_center, bool)
    CALL_MEMBER_X(nprims, std::size_t)
    CALL_MEMBER_X(primitive, auto)
};

#undef CALL_MEMBER_X

template<typename T>
struct IsGaussian : std::false_type {};
template<typename T>
struct IsGaussian<Gaussian<T>> : std::true_type {};
template<typename T, typename U>
struct IsGaussian<GaussianAlias<T, U>> : std::true_type {};

template<typename T>
static constexpr bool is_gaussian_v = IsGaussian<T>::value;

template<typename T>
using enable_if_gaussian_t = std::enable_if_t<is_gaussian_v<T>, int>;

template<typename T>
using disable_if_gaussian_t =
  std::enable_if_t<std::negation_v<IsGaussian<T>>, int>;

} // namespace detail_
} // namespace libchemist::basis_set
