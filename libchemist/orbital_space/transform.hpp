#pragma once
#include "libchemist/orbital_space/orbital_space.hpp"

namespace libchemist {
namespace detail_ {
#define AO_SPACE      const libchemist::orbital_space::AOSpace
#define DERIVED_SPACE const libchemist::orbital_space::DerivedSpace

/** @brief Determines which transformation results in the smallest tensor
 *
 *  Transformations are assumed to be new space on the columns and old space on
 *  the rows.
 *
 * @tparam T1
 * @tparam T2
 * @param s1
 * @param s2
 * @return 0 (false) if s1 should be applied first or 1 (true) if s2 should be
 *         applied first.
 */
template<typename T1, typename T2>
bool do_which_first(DERIVED_SPACE<T1>& s1, DERIVED_SPACE<T2>& s2) {
    // Figure out which one results in the smallest tensor
    auto s1_extent = s1.C().elements_range().extent();
    auto s2_extent = s2.C().elements_range().extent();

    // A transformation shrinks the tensor if the columns are less than the rows
    const bool s1_shrinks = s1_extent[1] < s1_extent[0];
    const bool s2_shrinks = s2_extent[1] < s2_extent[0];

    if(s1_shrinks && s2_shrinks) { // Both shrink, which shrinks the most?
        return s2_extent[1] < s1_extent[1];
    }
    else if(s1_shrinks) { return false;} // Only s1 shrinks
    else if(s2_shrinks) { return true; } // ONly s2 shrinks
    else{ // Both actually enlarge the tensor, which enlarges the least?
        return s2_extent[1] > s1_extent[1];
    }
}

template<typename T1, typename T2, typename T3>
std::size_t do_which_first(DERIVED_SPACE<T1>& s1, DERIVED_SPACE<T2>& s2,
                           DERIVED_SPACE<T3>& s3) {
    const bool s2_before_s1 = do_which_first(s1, s2);
    if(s2_before_s1) {
        const bool s2_before_s3 = do_which_first(s3, s2);
        if(s2_before_s3) return 1;
        else return 2;
    }
    const bool s1_before_s3 = do_which_first(s3, s1);
    if(s1_before_s3) return 0;
    return 2;
}

} // namespace detail_

//------------------------------------------------------------------------------
//                      Matrix Transformations
//------------------------------------------------------------------------------

template<typename T1, typename T2, typename TensorType>
auto transform(AO_SPACE<T1>&, AO_SPACE<T2>&, const TensorType& t) { return t; }

template<typename T1, typename T2, typename TensorType>
auto transform(DERIVED_SPACE<T1>& b, AO_SPACE<T2>&, const TensorType& t){
    TensorType rv;
    rv("k,j") = b.C()("i,k") * t("i,j");
    return rv;
}

template<typename T1, typename T2, typename TensorType>
auto transform(AO_SPACE<T1>&, DERIVED_SPACE<T2>& k, const TensorType& t) {
    TensorType rv;
    rv("i,k") = t("i,j") * k.C()("j,k");
    return rv;
}

template<typename T1, typename T2, typename TensorType>
auto transform(DERIVED_SPACE<T1>& b, DERIVED_SPACE<T2>& k, const TensorType& t) {
    const bool do_k_first = detail_::do_which_first(b, k);
    if(do_k_first) {
        auto temp = transform(b.from_space(), k, t);
        return transform(b, k.from_space(), temp);
    }
    auto temp = transform(b, k.from_space(), t);
    return transform(b.from_space(), k, temp);
}

//------------------------------------------------------------------------------
//                        Rank 3 Transforms
//------------------------------------------------------------------------------
template<typename T1, typename T2, typename T3, typename TensorType>
auto transform(AO_SPACE<T1>&, AO_SPACE<T2>&, AO_SPACE<T3>&, const TensorType& t) {
    return t;
}

template<typename T1, typename T2, typename T3, typename TensorType>
auto transform(DERIVED_SPACE<T1>& s0, AO_SPACE<T2>&, AO_SPACE<T3>&, const TensorType& t) {
    TensorType rv;
    rv("l,j,k") = s0.C()("i,l") * t("i,j,k");
    return rv;
}

template<typename T1, typename T2, typename T3, typename TensorType>
auto transform(AO_SPACE<T1>&, DERIVED_SPACE<T2>& s1, AO_SPACE<T3>&, const TensorType& t) {
    TensorType rv;
    rv("i,l,k") = s1.C()("j,l") * t("i,j,k");
    return rv;
}

template<typename T1, typename T2, typename T3, typename TensorType>
auto transform(AO_SPACE<T1>&, AO_SPACE<T2>&, DERIVED_SPACE<T3>& s2, const TensorType& t) {
    TensorType rv;
    rv("i,j,l") = t("i,j,k") * s2.C()("k,l");
    return rv;
}

template<typename T1, typename T2, typename T3, typename TensorType>
auto transform(DERIVED_SPACE<T1>& s0, DERIVED_SPACE<T2>& s1, AO_SPACE<T3>& s2, const TensorType& t) {
    const bool do_s1_first = detail_::do_which_first(s0, s1);
    if(do_s1_first) {
        auto temp = transform(s0.from_space(), s1, s2, t);
        return transform(s0, s1.from_space(), s2, temp);
    }
    auto temp = transform(s0, s1.from_space(), s2, t);
    return transform(s0.from_space(), s1, s2, temp);
}

template<typename T1, typename T2, typename T3, typename TensorType>
auto transform(DERIVED_SPACE<T1>& s0, AO_SPACE<T2>& s1, DERIVED_SPACE<T3>& s2, const TensorType& t) {
    const bool do_s2_first = detail_::do_which_first(s0, s2);
    if(do_s2_first) {
        auto temp = transform(s0.from_space(), s1, s2, t);
        return transform(s0, s1, s2.from_space(), temp);
    }
    auto temp = transform(s0, s1, s2.from_space(), t);
    return transform(s0.from_space(), s1, s2, temp);
}

template<typename T1, typename T2, typename T3, typename TensorType>
auto transform(AO_SPACE<T1>& s0, DERIVED_SPACE<T2>& s1, DERIVED_SPACE<T3>& s2, const TensorType& t) {
    const bool do_s2_first = detail_::do_which_first(s1, s2);
    if(do_s2_first) {
        auto temp = transform(s0, s1.from_space(), s2, t);
        return transform(s0, s1, s2.from_space(), temp);
    }
    auto temp = transform(s0, s1, s2.from_space(), t);
    return transform(s0, s1.from_space(), s2, temp);
}

template<typename T1, typename T2, typename T3, typename TensorType>
auto transform(DERIVED_SPACE<T1>& s0, DERIVED_SPACE<T2>& s1, DERIVED_SPACE<T3>& s2, const TensorType& t) {
    switch(detail_::do_which_first(s0, s1, s2)){
        case 0: {
            auto temp = transform(s0, s1.from_space(), s2.from_space(), t);
            return transform(s0.from_space(), s1, s2, temp);
        }
        case 1: {
            auto temp = transform(s0.from_space(), s1, s2.from_space(), t);
            return transform(s0, s1.from_space(), s2, temp);
        }
        default: {
            auto temp = transform(s0.from_space(), s1.from_space(), s2, t);
            return transform(s0, s1, s2.from_space(), t);
        }
    }
}

//------------------------------------------------------------------------------
//                       Rank 4 Transformations
//------------------------------------------------------------------------------

template<typename T1, typename T2, typename T3, typename T4, typename TensorType>
auto transform(AO_SPACE<T1>&, AO_SPACE<T2>&, AO_SPACE<T3>&, AO_SPACE<T4>&,
               const TensorType& t) {
    return t;
}

// One Derived
template<typename T1, typename T2, typename T3, typename T4, typename TensorType>
auto transform(DERIVED_SPACE<T1>& b1, AO_SPACE<T2>&, AO_SPACE<T3>&, AO_SPACE<T4>&,
               const TensorType& t) {
    TensorType rv;
    rv("m,j,k,l") = b1.C()("i,m") * t("i,j,k,l");
    return rv;
}

template<typename T1, typename T2, typename T3, typename T4, typename TensorType>
auto transform(AO_SPACE<T1>&, DERIVED_SPACE<T2>& b2, AO_SPACE<T3>&, AO_SPACE<T4>&,
               const TensorType& t) {
    TensorType rv;
    rv("i,m,k,l") = b2.C()("j,m") * t("i,j,k,l");
    return rv;
}

template<typename T1, typename T2, typename T3, typename T4, typename TensorType>
auto transform(AO_SPACE<T1>&, AO_SPACE<T2>&, DERIVED_SPACE<T3>& k1,
               AO_SPACE<T4>&, const TensorType& t) {
    TensorType rv;
    rv("i,j,m,l") = k1.C()("k,m") * t("i,j,k,l");
    return rv;
}

template<typename T1, typename T2, typename T3, typename T4, typename TensorType>
auto transform(AO_SPACE<T1>&, AO_SPACE<T2>&, AO_SPACE<T3>&,
               DERIVED_SPACE<T4>& k2, const TensorType& t) {
    TensorType rv;
    rv("i,j,k,m") = t("i,j,k,l") * k2.C()("l, m");
    return rv;
}

// Two Derived
template<typename T1, typename T2, typename T3, typename T4, typename TensorType>
auto transform(DERIVED_SPACE<T1>& b1, DERIVED_SPACE<T2>& b2,
               AO_SPACE<T3>& k1, AO_SPACE<T4>& k2, const TensorType& t) {
    const bool do_b2_first = detail_::do_which_first(b1, b2);
    if(do_b2_first){
        auto temp = transform(b1.from_space(), b2, k1, k2, t);
        return transform(b1, b2.from_space(), k1, k2, temp);
    }
    auto temp = transform(b1, b2.from_space(), k1, k2, t);
    return transform(b1.from_space(), b2, k1, k2, temp);
}

template<typename T1, typename T2, typename T3, typename T4, typename TensorType>
auto transform(DERIVED_SPACE<T1>& b1, AO_SPACE<T2>& b2,
               DERIVED_SPACE<T3>& k1, AO_SPACE<T4>& k2, const TensorType& t) {
    const bool do_k1_first = detail_::do_which_first(b1, k1);
    if(do_k1_first){
        auto temp = transform(b1.from_space(), b2, k1, k2, t);
        return transform(b1, b2, k1.from_space(), k2, temp);
    }
    auto temp = transform(b1, b2, k1.from_space(), k2, t);
    return transform(b1.from_space(), b2, k1, k2, temp);
}

template<typename T1, typename T2, typename T3, typename T4, typename TensorType>
auto transform(DERIVED_SPACE<T1>& b1, AO_SPACE<T2>& b2,
               AO_SPACE<T3>& k1, DERIVED_SPACE<T4>& k2, const TensorType& t) {
    const bool do_k2_first = detail_::do_which_first(b1, k2);
    if(do_k2_first){
        auto temp = transform(b1.from_space(), b2, k1, k2, t);
        return transform(b1, b2, k1, k2.from_space(), temp);
    }
    auto temp = transform(b1, b2, k1, k2.from_space(), t);
    return transform(b1.from_space(), b2, k1, k2, temp);
}

template<typename T1, typename T2, typename T3, typename T4, typename TensorType>
auto transform(AO_SPACE<T1>& b1, DERIVED_SPACE<T2>& b2,
               DERIVED_SPACE<T3>& k1, AO_SPACE<T4>& k2, const TensorType& t) {
    const bool do_k1_first = detail_::do_which_first(b2, k1);
    if(do_k1_first){
        auto temp = transform(b1, b2.from_space(), k1, k2, t);
        return transform(b1, b2, k1.from_space(), k2, temp);
    }
    auto temp = transform(b1, b2, k1.from_space(), k2, t);
    return transform(b1, b2.from_space(), k1, k2, temp);
}

template<typename T1, typename T2, typename T3, typename T4, typename TensorType>
auto transform(AO_SPACE<T1>& b1, DERIVED_SPACE<T2>& b2,
               AO_SPACE<T3>& k1, DERIVED_SPACE<T4>& k2, const TensorType& t) {
    const bool do_k2_first = detail_::do_which_first(b2, k2);
    if(do_k2_first){
        auto temp = transform(b1, b2.from_space(), k1, k2, t);
        return transform(b1, b2, k1, k2.from_space(), temp);
    }
    auto temp = transform(b1, b2, k1, k2.from_space(), t);
    return transform(b1, b2.from_space(), k1, k2, temp);
}

template<typename T1, typename T2, typename T3, typename T4, typename TensorType>
auto transform(AO_SPACE<T1>& b1, AO_SPACE<T2>& b2,
               DERIVED_SPACE<T3>& k1, DERIVED_SPACE<T4>& k2, const TensorType& t) {
    const bool do_k2_first = detail_::do_which_first(k1, k2);
    if(do_k2_first){
        auto temp = transform(b1, b2, k1.from_space(), k2, t);
        return transform(b1, b2, k1, k2.from_space(), temp);
    }
    auto temp = transform(b1, b2, k1, k2.from_space(), t);
    return transform(b1, b2, k1.from_space(), k2, temp);
}

// Three derived
template<typename T1, typename T2, typename T3, typename T4, typename TensorType>
auto transform(DERIVED_SPACE<T1>& b1, DERIVED_SPACE<T2>& b2,
               DERIVED_SPACE<T3>& k1, AO_SPACE<T4>& k2, const TensorType& t) {
    switch(detail_::do_which_first(b1, b2, k1)){
        case 0: {
            auto temp = transform(b1, b2.from_space(), k1.from_space(), k2, t);
            return tranform(b1.from_space(), b2, k1, k2, temp);
        }
        case 1: {
            auto temp = transform(b1.from_space(), b2, k1.from_space(), k2, t);
            return transform(b1, b2.from_space(), k1, k2, temp);
        }
        default: {
            auto temp = transform(b1.from_space(), b2.from_space(), k1, k2, t);
            return transform(b1, b2, k1.from_space(), k2, temp);
        }
    }
}

template<typename T1, typename T2, typename T3, typename T4, typename TensorType>
auto transform(DERIVED_SPACE<T1>& b1, DERIVED_SPACE<T2>& b2,
               AO_SPACE<T3>& k1, DERIVED_SPACE<T4>& k2, const TensorType& t) {
    switch(detail_::do_which_first(b1, b2, k2)){
        case 0: {
            auto temp = transform(b1, b2.from_space(), k1, k2.from_space(), t);
            return tranform(b1.from_space(), b2, k1, k2, temp);
        }
        case 1: {
            auto temp = transform(b1.from_space(), b2, k1, k2.from_space(), t);
            return transform(b1, b2.from_space(), k1, k2, temp);
        }
        default: {
            auto temp = transform(b1.from_space(), b2.from_space(), k1, k2, t);
            return transform(b1, b2, k1, k2.from_space(), temp);
        }
    }
}

template<typename T1, typename T2, typename T3, typename T4, typename TensorType>
auto transform(DERIVED_SPACE<T1>& b1, AO_SPACE<T2>& b2,
               DERIVED_SPACE<T3>& k1, DERIVED_SPACE<T4>& k2, const TensorType& t) {
    switch(detail_::do_which_first(b1, k1, k2)){
        case 0: {
            auto temp = transform(b1, b2, k1.from_space(), k2.from_space(), t);
            return tranform(b1.from_space(), b2, k1, k2, temp);
        }
        case 1: {
            auto temp = transform(b1.from_space(), b2, k1, k2.from_space(), t);
            return transform(b1, b2, k1.from_space(), k2, temp);
        }
        default: {
            auto temp = transform(b1.from_space(), b2, k1.from_space(), k2, t);
            return transform(b1, b2, k1, k2.from_space(), temp);
        }
    }
}

template<typename T1, typename T2, typename T3, typename T4, typename TensorType>
auto transform(AO_SPACE<T1>& b1, DERIVED_SPACE<T2>& b2,
               DERIVED_SPACE<T3>& k1, DERIVED_SPACE<T4>& k2, const TensorType& t) {
    switch(detail_::do_which_first(b2, k1, k2)){
        case 0: {
            auto temp = transform(b1, b2, k1.from_space(), k2.from_space(), t);
            return tranform(b1, b2.from_space(), k1, k2, temp);
        }
        case 1: {
            auto temp = transform(b1, b2.from_space(), k1, k2.from_space(), t);
            return transform(b1, b2, k1.from_space(), k2, temp);
        }
        default: {
            auto temp = transform(b1, b2.from_space(), k1.from_space(), k2, t);
            return transform(b1, b2, k1, k2.from_space(), temp);
        }
    }
}

} // namespace libchemist
