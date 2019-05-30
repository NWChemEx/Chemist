#pragma once
#include <tuple>
namespace libchemist::basis_set::detail_ {

template<typename T, typename U>
constexpr bool compare_gaussians(T&& lhs, U&& rhs) noexcept {
    const auto lhs1 = std::tie(lhs.coefficient(), lhs.exponent());
    const auto rhs1 = std::tie(rhs.coefficient(), rhs.exponent());
    if(lhs1 != rhs1) return false;

    for(std::size_t i = 0; i < 3; ++i)
        if(lhs.center()[i] != rhs.center()[i]) return false;
    return true;
}

template<typename T, typename U>
constexpr bool compare_ao(T&& lhs, U&& rhs) noexcept {
    if(lhs.nprims() != rhs.nprims()) return false;
    for(type::size i = 0; i < lhs.nprims(); ++i)
        if(lhs.primitive(i) != rhs.primitive(i)) return false;
    return true;
}

template<typename Idx, typename... Idxs>
constexpr auto cat_indices(Idx&& lhs, Idxs... idxs) noexcept {
    constexpr auto N = std::tuple_size_v<std::remove_reference_t<Idx>>;
    constexpr auto M = N + sizeof...(Idxs);
    return std::tuple_cat(lhs, std::array{idxs...});
}

#define CALL_MEMBER_X(fxn_name)                                                \
    namespace impl {                                                           \
    template<typename BS, typename Idx, std::size_t... Is>                     \
    constexpr auto& fxn_name(BS&& bs, Idx&& idx, std::index_sequence<Is...>) { \
        return bs->fxn_name(std::get<Is>(idx)...);                             \
    }                                                                          \
    }                                                                          \
    template<typename BS, typename Idx>                                        \
    constexpr auto& fxn_name(BS&& bs, Idx&& idx) {                             \
        constexpr auto N = std::tuple_size_v<std::remove_reference_t<Idx>>;    \
        return impl::fxn_name(std::forward<BS>(bs), std::forward<Idx>(idx),    \
                              std::make_index_sequence<N>{});                  \
    }

CALL_MEMBER_X(coefficient)
CALL_MEMBER_X(exponent)
CALL_MEMBER_X(center)
CALL_MEMBER_X(nprims)

#undef CALL_MEMBER_X

} // namespace libchemist::basis_set::detail_
