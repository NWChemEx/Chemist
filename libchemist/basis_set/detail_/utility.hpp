#pragma once
#include <libchemist/types.hpp>
#include <optional>
#include <tuple>

namespace libchemist::basis_set {
// Forward declare basis set classes for template meta-programming purposes
template<typename T>
class Gaussian;
template<typename T, typename U>
class GaussianAlias;
template<typename T>
class GaussianAO;
template<typename T, typename U>
class GaussianAOAlias;
template<typename T>
class GaussianShell;
template<typename T, typename U>
class GaussianShellAlias;

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
    if(lhs.naos() != rhs.naos()) return false;
    for(std::size_t i = 0; i < lhs.naos(); ++i)
        if(lhs.ao(i) != rhs.ao(i)) return false;
    return true;
}

template<typename T, typename U>
constexpr bool compare_center(T&& lhs, U&& rhs) noexcept {
    if(lhs.nshells() != rhs.nshells()) return false;
    for(std::size_t i = 0; i < lhs.nshells(); ++i)
        if(lhs.shell(i) != rhs.shell(i)) return false;
    return true;
}

constexpr inline std::size_t bc(std::size_t n, std::size_t k) noexcept {
    if(k == 0 || k == n) return 1;
    if(n == 0) return 0;
    return bc(n - 1, k - 1) + bc(n - 1, k);
}

template<typename Idx, typename... Idxs>
constexpr auto cat_indices(Idx&& lhs, Idxs... idxs) noexcept {
    constexpr auto N = std::tuple_size_v<std::remove_reference_t<Idx>>;
    constexpr auto M = N + sizeof...(Idxs);
    return std::tuple_cat(lhs, std::array{idxs...});
}

/** @brief Defines the redirection necessary to unpack an std::array of indices
 *         into a function, which expects individual indices.
 *
 *  There's probably a cleaner non-macro solution, using std::apply, but the
 *  member function nature of the functions made it a pain. Hence I broke down
 *  and wrote this macro. This macro is just std::apply with the name of the
 *  function to call hard-coded.
 *
 *  @param[in] fxn_name The name of the function
 *  @param[in] rv The return value of the function
 *
 */
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

/** @brief A struct to wrap calling the index member functions.
 *
 *  This is a class so you just have to friend the class and not each individual
 *  function.
 *
 */
struct CallMemberX {
    CALL_MEMBER_X(coefficient, auto&)
    CALL_MEMBER_X(exponent, auto&)
    CALL_MEMBER_X(center, auto&)
    CALL_MEMBER_X(has_center, bool)
    CALL_MEMBER_X(nprims, auto)
    CALL_MEMBER_X(primitive, auto)
    CALL_MEMBER_X(pure, auto&)
    CALL_MEMBER_X(l, auto&)
    CALL_MEMBER_X(naos, auto)
    CALL_MEMBER_X(ao, auto)
    CALL_MEMBER_X(shell, auto);
    CALL_MEMBER_X(nshells, auto);
};

#undef CALL_MEMBER_X

/** @brief Defines traits to determine if a type is of type @p X and
 *         enable/disable functions based on that check.
 *
 *  This macro defines `is_x_v<T>`, `enable_if_x_t<T>`, and `disable_if_x_t<T>`
 *  traits, where `x` is the value of @p x. Thes traits respectively tell you
 *  if the template parameter @p T is of type @p X, enable a function if @p T
 *  is of type @p X, and disable a function if @p T is of type @p X.
 *
 *  @param[in] X The name of the class in CamelCase
 *  @param[in] x The name of the class in snake_case
 */
#define CLASS_TRAITS(X, x)                                          \
    template<typename T>                                            \
    struct Is##X : std::false_type {};                              \
    template<typename T>                                            \
    struct Is##X<X<T>> : std::true_type {};                         \
    template<typename T, typename U>                                \
    struct Is##X<X##Alias<T, U>> : std::true_type {};               \
    template<typename T>                                            \
    static constexpr bool is_##x##_v = Is##X<T>::value;             \
    template<typename T>                                            \
    using enable_if_##x##_t = std::enable_if_t<is_##x##_v<T>, int>; \
    template<typename T>                                            \
    using disable_if_##x##_t =                                      \
      std::enable_if_t<std::negation_v<Is##X<T>>, int>

CLASS_TRAITS(Gaussian, gaussian);
CLASS_TRAITS(GaussianAO, gaussian_ao);
#undef CLASS_TRAITS

/// Determines the container to use to hold the provided primitives
template<typename PrimType, typename... Prims>
using ao_container_t =
  std::array<typename PrimType::value_type, 1 + sizeof...(Prims)>;

template<typename T>
struct GaussianAOContainer;

template<typename T>
struct GaussianAOContainer<GaussianAO<T>> {
    using type = T;
};
template<typename T, typename U>
struct GaussianAOContainer<GaussianAOAlias<T, U>> {
    using type = std::vector<typename GaussianAOAlias<T, U>::value_type>;
};

template<typename T>
using gaussian_ao_container_t = typename GaussianAOContainer<T>::type;

template<typename T>
struct GaussianShellContainer;

template<typename T>
struct GaussianShellContainer<GaussianShell<T>> {
    using type = T;
};

template<typename T, typename U>
struct GaussianShellContainer<GaussianShellAlias<T, U>> {
    using type = std::vector<typename GaussianShellAlias<T, U>::value_type>;
};

template<typename T>
using gaussian_shell_container_t = typename GaussianShellContainer<T>::type;

template<typename T>
struct IsArray : std::false_type {};

template<typename T, std::size_t N>
struct IsArray<std::array<T, N>> : std::true_type {};

template<typename T>
static constexpr bool is_array_v = IsArray<T>::value;

template<typename T, typename... Args>
struct SumArray;

template<typename T, typename... Args>
static constexpr std::size_t sum_array_v = SumArray<T, Args...>::value;

template<typename T>
struct SumArray<T> {
    static constexpr auto value = std::tuple_size_v<T>;
};

template<typename T, typename... Args>
struct SumArray {
    static constexpr auto value = std::tuple_size_v<T> + sum_array_v<Args...>;
};

template<typename... Shells>
struct GaussianCenterContainer {
    /// True if all of the shells use std::array
    static constexpr bool all_array =
      std::conjunction_v<IsArray<gaussian_shell_container_t<Shells>>...>;

    /// If they all use arrays, this is the total number of elements
    static constexpr std::size_t N =
      all_array ? sum_array_v<gaussian_shell_container_t<Shells>...> : 0;

    using s0_type =
      std::tuple_element_t<0,
                           std::tuple<gaussian_shell_container_t<Shells>...>>;

    using value_type = typename s0_type::value_type;
    using type       = std::conditional_t<all_array, std::array<value_type, N>,
                                    std::vector<value_type>>;
};

template<typename... Shells>
using gaussian_center_container_t =
  typename GaussianCenterContainer<Shells...>::type;

template<typename Sub>
constexpr auto get_ao_coefs(const Sub& s) {
    using T = gaussian_ao_container_t<std::decay_t<Sub>>;
    T coefs{};
    if constexpr(!IsArray<T>::value) { coefs = T(s.size()); }
    for(std::size_t i = 0; i < s.nprims(); ++i)
        coefs[i] = s.primitive(i).coefficient();
    return coefs;
}

template<typename Sub>
constexpr auto get_ao_exps(const Sub& s) {
    using T = gaussian_ao_container_t<std::decay_t<Sub>>;
    T coefs{};
    if constexpr(!IsArray<T>::value) { coefs = T(s.size()); }
    for(std::size_t i = 0; i < s.nprims(); ++i)
        coefs[i] = s.primitive(i).exponent();
    return coefs;
}

} // namespace detail_
} // namespace libchemist::basis_set
