#pragma once
#include "chemist/orbital_space/orbital_space.hpp"

/** @file transform.hpp
 *
 *  This file defines a function `transform` which takes a rank @f$r@f$ tensor
 *  and up to @f$r@f$ DerivedSpace instances. `transform` will then apply the
 *  transformations in the DerivedSpace instances to the tensor in the optimal
 *  order.
 *
 *  At the moment the overloads have all been written manually. It wouldn't be
 *  too hard to auto-generate them with a Python script. In theory it should be
 *  possible to unify them with some template meta-programming, but assuming we
 *  only need to transform up to a rank 4 tensor that's probably more work than
 *  it's worth. The number of overloads for a rank @f$r@f$ tensor is @f$2^r@f$
 *  so manually writing out the overloads quickly becomes impractical if we have
 *  to go much higher.
 */

namespace chemist {
namespace detail_ {

#define AO_SPACE const chemist::orbital_space::AOSpace
#define DERIVED_SPACE const chemist::orbital_space::DerivedSpace

/** @brief Determines which transformation results in the smallest tensor
 *
 *  This function compares two transformations and determines which one will
 *  create the smallest tensor. In the event that the two transformations result
 *  in the same size tensor this function will indicate that the first one
 *  should be applied first.
 *
 *  @note Transformations are assumed to be new space on the columns and old
 *        space on the rows.
 *
 *  @tparam T1 The scalar type of the first transformation.
 *  @tparam T2 The scalar type of the second transformation.
 *
 *  @param[in] s1 One of the two transformations to apply.
 *  @param[in] s2 The other transformation to apply.
 *
 *  @return 0 (false) if s1 should be applied first or 1 (true) if s2 should be
 *          applied first.
 *
 */
template<typename T1, typename T2>
bool do_which_first(DERIVED_SPACE<T1>& s1, DERIVED_SPACE<T2>& s2) {
    // We assume transformations are AOs on rows and MOs on columns and that we
    // are transforming from AOs to MOs (N.B. there's nothing here tied to AOs
    // or MOs)
    auto s1_extent    = s1.C().elements_range().extent();
    const auto s1_nao = s1_extent[0];
    const auto s1_nmo = s1_extent[1];

    auto s2_extent    = s2.C().elements_range().extent();
    const auto s2_nao = s2_extent[0];
    const auto s2_nmo = s2_extent[1];

    // These transformations can only change the size of two of the modes of the
    // tensor. When we start the tensor is s1_nao * s2_nao * x elements large
    //(x is the product of the extents of the remaining modes). If we apply
    // s1 first the result is s1_nmo * s2_nao * x and if we apply s2 first the
    // result is s1_nao * s2_nmo * x. Assuming all quantities are integral and
    // non-zero this means that if s1_nmo * s2_nao < s1_nao * s2_nmo we should
    // apply s1 first. If s1_nmo * s2_nao > s1_nao * s2_nmo we should apply s2
    // first. And if they're the same it doesn't matter which one gets applied
    // first.
    const auto s1_first = s1_nmo * s2_nao;
    const auto s2_first = s1_nao * s2_nmo;
    return s2_first < s1_first;
}

/** @brief Compares three DerivedSpaces and determines which to apply first.
 *
 *  This function will look at the three supplied transformations and determine
 *  which one results in the smallest tensor. Users can use the two-space
 *  overload to determine which space to apply second (and by process of
 *  elimination, which to apply third). In the event that there is a tie for
 *  smallest the lower number is returned (i.e. if arguments 0 and 1 or 0 and 2
 *  tie 0 is returned, whereas if arguments 1 and 2 tie 1 is returned).
 *
 *  @tparam T1 The scalar type of the first transformation.
 *  @tparam T2 The scalar type of the second transformation.
 *  @tparam T3 The scalar type of the third transformation.
 *
 *  @param[in] s1 The space associated with the first transform.
 *  @param[in] s2 The space associated with the second transform.
 *  @param[in] s3 The space associated with the thrid transform.
 *
 *  @return 0, 1, or 2 if @p s1, @p s2, or @p s3 should respectively be applied
 *          first.
 */
template<typename T1, typename T2, typename T3>
std::size_t do_which_first(DERIVED_SPACE<T1>& s1, DERIVED_SPACE<T2>& s2,
                           DERIVED_SPACE<T3>& s3) {
    // This first part is conceptually all the same as the two input overload.
    // See the comments there for more help.

    auto s1_extent    = s1.C().elements_range().extent();
    const auto s1_nao = s1_extent[0];
    const auto s1_nmo = s1_extent[1];

    auto s2_extent    = s2.C().elements_range().extent();
    const auto s2_nao = s2_extent[0];
    const auto s2_nmo = s2_extent[1];

    auto s3_extent    = s3.C().elements_range().extent();
    const auto s3_nao = s3_extent[0];
    const auto s3_nmo = s3_extent[1];

    const auto s1_first = s1_nmo * s2_nao * s3_nao;
    const auto s2_first = s1_nao * s2_nmo * s3_nao;
    const auto s3_first = s1_nao * s2_nao * s3_nmo;

    // Theres's six possibilities:
    // s1 <= s2 <= s3
    // s1 <= s3 < s2 (s3 == s2 accounted for above)
    // s2 < s1 <= s3 (s2 == s1 accounted for above)
    // s2 <= s3 < s1 (s3 == s1 accounted for above)
    // s3 < s1 <= s2 (s3 == s1 accounted for above)
    // s3 < s2 < s1  (s3 == s1 and/or s2 == s1 accounted for above)
    if(s1_first <= s2_first) {
        // if ternary condition is true we have s3 < s1 <= s2
        // if false, we have s1 <= s3 && s1 <= s2, which covers
        // s1 <= s2 <= s3 && s1 <= s3 < s2
        return s3_first < s1_first ? 2 : 0;
    }
    // We know s2 < s1. If ternary condition is true we have s2 < s1 &&
    // s2 <= s3, which covers s2 < s1 <= s3 && s2 <= s3 < s1. If false, we have
    // s3 < s2 < s1
    return s2_first <= s3_first ? 1 : 2;
}

template<typename T1, typename T2, typename T3, typename T4>
std::size_t do_which_first(DERIVED_SPACE<T1>& s1, DERIVED_SPACE<T2>& s2,
                           DERIVED_SPACE<T3>& s3, DERIVED_SPACE<T4>& s4) {
    auto first = do_which_first(s1, s2, s3);
    if(first == 0) {
        // we know s1 <= s2 && s1 <= s3. If condition is false we also have
        // s1 < s4. If true we have s4 <= s1 <= s2 && s4 <= s1 <= s3.
        const bool s4_before_s1 = do_which_first(s1, s4);
        return s4_before_s1 ? 3 : 0;
    } else if(first == 1) {
        // we know s2 < s1  && s2 <= s3. If condition is true we also have
        // s2 <= s4. If false we have s4 < s2 < s1 && s4 < s2 <= s3.
        const bool s2_before_s4 = do_which_first(s4, s2);
        return s2_before_s4 ? 1 : 3;
    }
    // we know s3 < s1 and s3 < s2. If condition is true we also have
    // s3 <= s4. If false we have s4 < s3 < s1 and s4 < s3 < s2.
    const bool s3_before_s4 = do_which_first(s4, s3);
    return s3_before_s4 ? 2 : 3;
}

} // namespace detail_

//------------------------------------------------------------------------------
//                      Matrix Transformations
//------------------------------------------------------------------------------

/** @brief Transforms a matrix.
 *
 *  This overload exists for recursion's sake. It is a no-op and simply returns
 *  the tensor provided to it.
 *
 * @tparam T1 The scalar type of the first transformation
 * @tparam T2 The scalar type of the second transformation
 * @tparam TensorType The type of the tensor being transformed
 *
 * @param[in] t The tensor to transform
 *
 * @return @p t unchanged.
 */
template<typename T1, typename T2, typename TensorType>
auto transform(AO_SPACE<T1>&, AO_SPACE<T2>&, const TensorType& t) {
    return t;
}

/** @brief Transforms a matrix.
 *
 *  This overload applies the transformation found in the first argument to the
 *  provided tensor.
 *
 * @tparam T1 The scalar type of the first transformation
 * @tparam T2 The scalar type of the second transformation
 * @tparam TensorType The type of the tensor being transformed
 *
 * @param[in] b The transformation to apply to the 0-th mode of @p t.
 * @param[in] t The tensor to transform
 *
 * @return @p t with it's 0-th mode transformed by `b.C()`
 */
template<typename T1, typename T2, typename TensorType>
auto transform(DERIVED_SPACE<T1>& b, AO_SPACE<T2>&, const TensorType& t) {
    TensorType rv;
    rv("k,j") = b.C()("i,k") * t("i,j");
    return rv;
}

/** @brief Transforms a matrix.
 *
 *  This overload applies the transformation found in the second argument to the
 *  provided tensor.
 *
 * @tparam T1 The scalar type of the first transformation
 * @tparam T2 The scalar type of the second transformation
 * @tparam TensorType The type of the tensor being transformed
 *
 * @param[in] k The transformation to apply to the 1-st mode of @p t.
 * @param[in] t The tensor to transform
 *
 * @return @p t with it's 1-st mode transformed by `k.C()`
 */
template<typename T1, typename T2, typename TensorType>
auto transform(AO_SPACE<T1>&, DERIVED_SPACE<T2>& k, const TensorType& t) {
    TensorType rv;
    rv("i,k") = t("i,j") * k.C()("j,k");
    return rv;
}

/** @brief Transforms a matrix.
 *
 *  This overload applies the transformation found in the first argument to the
 *  0-th mode of the provided tensor and the transformation found in the
 *  second argument to the 1-st mode of the tensor.
 *
 * @tparam T1 The scalar type of the first transformation
 * @tparam T2 The scalar type of the second transformation
 * @tparam TensorType The type of the tensor being transformed
 *
 * @param[in] b The transformation to apply to the 0-th mode of @p t.
 * @param[in] k The transformation to apply to the 1-st mode of @p t.
 * @param[in] t The tensor to transform
 *
 * @return @p t with it's 0-th mode transformed by `b.C()` and it's 1-st mode
 *         transformed by `k.C()`.
 */
template<typename T1, typename T2, typename TensorType>
auto transform(DERIVED_SPACE<T1>& b, DERIVED_SPACE<T2>& k,
               const TensorType& t) {
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

/** @brief Transforms a rank 3 tensor.
 *
 *  This overload exists for recursion's sake. It is a no-op and simply returns
 *  the tensor provided to it.
 *
 * @tparam T1 The scalar type of the first transformation
 * @tparam T2 The scalar type of the second transformation
 * @tparam T3 The scalar type of the third transformation
 * @tparam TensorType The type of the tensor being transformed
 *
 * @param[in] t The tensor to transform
 *
 * @return @p t unchanged.
 */
template<typename T1, typename T2, typename T3, typename TensorType>
auto transform(AO_SPACE<T1>&, AO_SPACE<T2>&, AO_SPACE<T3>&,
               const TensorType& t) {
    return t;
}

/** @brief Transforms a rank 3 tensor.
 *
 *  This overload applies the provided transformation to the 0-th mode of the
 *  tensor.
 *
 * @tparam T1 The scalar type of the first transformation
 * @tparam T2 The scalar type of the second transformation
 * @tparam T3 The scalar type of the third transformation
 * @tparam TensorType The type of the tensor being transformed
 *
 * @param[in] s0 The orbital space whose transformation will be applied to the
 *               0-th mode.
 * @param[in] t The tensor to transform
 *
 * @return @p t with its 0-th mode transformed according to @p s0.
 */
template<typename T1, typename T2, typename T3, typename TensorType>
auto transform(DERIVED_SPACE<T1>& s0, AO_SPACE<T2>&, AO_SPACE<T3>&,
               const TensorType& t) {
    TensorType rv;
    rv("l,j,k") = s0.C()("i,l") * t("i,j,k");
    return rv;
}

/** @brief Transforms a rank 3 tensor.
 *
 *  This overload applies the provided transformation to the 1-st mode of the
 *  tensor.
 *
 * @tparam T1 The scalar type of the first transformation
 * @tparam T2 The scalar type of the second transformation
 * @tparam T3 The scalar type of the third transformation
 * @tparam TensorType The type of the tensor being transformed
 *
 * @param[in] s1 The orbital space whose transformation will be applied to the
 *               1-st mode.
 * @param[in] t The tensor to transform
 *
 * @return @p t with its 1-st mode transformed according to @p s1.
 */
template<typename T1, typename T2, typename T3, typename TensorType>
auto transform(AO_SPACE<T1>&, DERIVED_SPACE<T2>& s1, AO_SPACE<T3>&,
               const TensorType& t) {
    TensorType rv;
    rv("i,l,k") = s1.C()("j,l") * t("i,j,k");
    return rv;
}

/** @brief Transforms a rank 3 tensor.
 *
 *  This overload applies the provided transformation to the 2-nd mode of the
 *  tensor.
 *
 * @tparam T1 The scalar type of the first transformation
 * @tparam T2 The scalar type of the second transformation
 * @tparam T3 The scalar type of the third transformation
 * @tparam TensorType The type of the tensor being transformed
 *
 * @param[in] s2 The orbital space whose transformation will be applied to the
 *               2-nd mode.
 * @param[in] t The tensor to transform
 *
 * @return @p t with its 2-nd mode transformed according to @p s2.
 */
template<typename T1, typename T2, typename T3, typename TensorType>
auto transform(AO_SPACE<T1>&, AO_SPACE<T2>&, DERIVED_SPACE<T3>& s2,
               const TensorType& t) {
    TensorType rv;
    rv("i,j,l") = t("i,j,k") * s2.C()("k,l");
    return rv;
}

/** @brief Transforms a rank 3 tensor.
 *
 *  This overload applies the provided transformations to the 0-th and 1-st
 *  modes of the tensor.
 *
 * @tparam T1 The scalar type of the first transformation
 * @tparam T2 The scalar type of the second transformation
 * @tparam T3 The scalar type of the third transformation
 * @tparam TensorType The type of the tensor being transformed
 *
 * @param[in] s0 The orbital space whose transformation will be applied to the
 *               0-th mode.
 * @param[in] s1 The orbital space whose transformation will be applied to the
 *               1-st mode.
 * @param[in] s2 The orbital space spanning the 2-nd mode of the tensor.
 * @param[in] t The tensor to transform
 *
 * @return @p t with its 0-th mode transformed according to @p s0 and it's 1-st
 *         mode transformed according to @p s1.
 */
template<typename T1, typename T2, typename T3, typename TensorType>
auto transform(DERIVED_SPACE<T1>& s0, DERIVED_SPACE<T2>& s1, AO_SPACE<T3>& s2,
               const TensorType& t) {
    const bool do_s1_first = detail_::do_which_first(s0, s1);
    if(do_s1_first) {
        auto temp = chemist::transform(s0.from_space(), s1, s2, t);
        return chemist::transform(s0, s1.from_space(), s2, temp);
    }
    auto temp = chemist::transform(s0, s1.from_space(), s2, t);
    return chemist::transform(s0.from_space(), s1, s2, temp);
}

/** @brief Transforms a rank 3 tensor.
 *
 *  This overload applies the provided transformations to the 0-th and 2-nd
 *  modes of the tensor.
 *
 * @tparam T1 The scalar type of the first transformation
 * @tparam T2 The scalar type of the second transformation
 * @tparam T3 The scalar type of the third transformation
 * @tparam TensorType The type of the tensor being transformed
 *
 * @param[in] s0 The orbital space whose transformation will be applied to the
 *               0-th mode.
 * @param[in] s1 The orbital space spanning the 1-st mode of the tensor.
 * @param[in] s2 The orbital space whose transformation will be applied to the
 *               2-nd mode.
 * @param[in] t The tensor to transform
 *
 * @return @p t with its 0-th mode transformed according to @p s0 and it's 2-nd
 *         mode transformed according to @p s2.
 */
template<typename T1, typename T2, typename T3, typename TensorType>
auto transform(DERIVED_SPACE<T1>& s0, AO_SPACE<T2>& s1, DERIVED_SPACE<T3>& s2,
               const TensorType& t) {
    const bool do_s2_first = detail_::do_which_first(s0, s2);
    if(do_s2_first) {
        auto temp = chemist::transform(s0.from_space(), s1, s2, t);
        return chemist::transform(s0, s1, s2.from_space(), temp);
    }
    auto temp = chemist::transform(s0, s1, s2.from_space(), t);
    return chemist::transform(s0.from_space(), s1, s2, temp);
}

/** @brief Transforms a rank 3 tensor.
 *
 *  This overload applies the provided transformations to the 1-st and 2-nd
 *  modes of the tensor.
 *
 * @tparam T1 The scalar type of the first transformation
 * @tparam T2 The scalar type of the second transformation
 * @tparam T3 The scalar type of the third transformation
 * @tparam TensorType The type of the tensor being transformed
 *
 * @param[in] s0 The orbital space spanning the 0-th mode of the tensor.
 * @param[in] s1 The orbital space whose transformation will be applied to the
 *               1-st mode.
 * @param[in] s2 The orbital space whose transformation will be applied to the
 *               2-nd mode.
 * @param[in] t The tensor to transform
 *
 * @return @p t with its 1-st mode transformed according to @p s1 and it's 2-nd
 *         mode transformed according to @p s2.
 */
template<typename T1, typename T2, typename T3, typename TensorType>
auto transform(AO_SPACE<T1>& s0, DERIVED_SPACE<T2>& s1, DERIVED_SPACE<T3>& s2,
               const TensorType& t) {
    const bool do_s2_first = detail_::do_which_first(s1, s2);
    if(do_s2_first) {
        auto temp = chemist::transform(s0, s1.from_space(), s2, t);
        return chemist::transform(s0, s1, s2.from_space(), temp);
    }
    auto temp = chemist::transform(s0, s1, s2.from_space(), t);
    return chemist::transform(s0, s1.from_space(), s2, temp);
}

/** @brief Transforms a rank 3 tensor.
 *
 *  This overload applies the provided transformations to the 0-th, 1-st, and
 *  2-nd modes of the tensor.
 *
 * @tparam T1 The scalar type of the first transformation
 * @tparam T2 The scalar type of the second transformation
 * @tparam T3 The scalar type of the third transformation
 * @tparam TensorType The type of the tensor being transformed
 *
 * @param[in] s0 The orbital space whose transformation will be applied to the
 *               0-th mode.
 * @param[in] s1 The orbital space whose transformation will be applied to the
 *               1-st mode.
 * @param[in] s2 The orbital space whose transformation will be applied to the
 *               2-nd mode.
 * @param[in] t The tensor to transform
 *
 * @return @p t with its 0-th mode transformed according to @p s0, it's 1-st
 *         mode transformed according to @p s1, and it's 2-nd mode transformed
 *         according to @p s2.
 */
template<typename T1, typename T2, typename T3, typename TensorType>
auto transform(DERIVED_SPACE<T1>& s0, DERIVED_SPACE<T2>& s1,
               DERIVED_SPACE<T3>& s2, const TensorType& t) {
    const auto& aos0 = s0.from_space();
    const auto& aos1 = s1.from_space();
    const auto& aos2 = s2.from_space();

    switch(detail_::do_which_first(s0, s1, s2)) {
        case 0: {
            auto temp = chemist::transform(s0, aos1, aos2, t);
            return chemist::transform(aos0, s1, s2, temp);
        }
        case 1: {
            auto temp = chemist::transform(aos0, s1, aos2, t);
            return chemist::transform(s0, aos1, s2, temp);
        }
        default: {
            auto temp = chemist::transform(aos0, aos1, s2, t);
            return chemist::transform(s0, s1, aos2, t);
        }
    }
}

//------------------------------------------------------------------------------
//                       Rank 4 Transformations
//------------------------------------------------------------------------------

/** @brief Transforms a rank 4 tensor
 *
 *  This function is primarily for recursion termination and is a no-op
 *
 * @tparam T1 The scalar type of the transformation for the 0-th mode
 * @tparam T2 The scalar type of the transformation for the 1-st mode
 * @tparam T3 The scalar type of the transformation for the 2-nd mode
 * @tparam T4 The scalar type of the transformation for the 3-rd mode
 * @tparam TensorType The type of the tensor being transformed
 *
 * @param[in] t The tensor to transform
 *
 * @return @p t untouched
 */
template<typename T1, typename T2, typename T3, typename T4,
         typename TensorType>
auto transform(AO_SPACE<T1>&, AO_SPACE<T2>&, AO_SPACE<T3>&, AO_SPACE<T4>&,
               const TensorType& t) {
    return t;
}

/** @brief Transforms a rank 4 tensor
 *
 *  Applies the provided transformation to the 0-th mode of a tensor.
 *
 * @tparam T1 The scalar type of the transformation for the 0-th mode
 * @tparam T2 The scalar type of the transformation for the 1-st mode
 * @tparam T3 The scalar type of the transformation for the 2-nd mode
 * @tparam T4 The scalar type of the transformation for the 3-rd mode
 * @tparam TensorType The type of the tensor being transformed
 *
 * @param[in] b1 The orbital space to transform mode 0 to
 * @param[in] t The tensor to transform
 *
 * @return @p t with the 0-th mode transformed according to @p b1.
 */
template<typename T1, typename T2, typename T3, typename T4,
         typename TensorType>
auto transform(DERIVED_SPACE<T1>& b1, AO_SPACE<T2>&, AO_SPACE<T3>&,
               AO_SPACE<T4>&, const TensorType& t) {
    TensorType rv;
    rv("m,j,k,l") = b1.C()("i,m") * t("i,j,k,l");
    return rv;
}

/** @brief Transforms a rank 4 tensor
 *
 *  Applies the provided transformation to the 1-st mode of a tensor.
 *
 * @tparam T1 The scalar type of the transformation for the 0-th mode
 * @tparam T2 The scalar type of the transformation for the 1-st mode
 * @tparam T3 The scalar type of the transformation for the 2-nd mode
 * @tparam T4 The scalar type of the transformation for the 3-rd mode
 * @tparam TensorType The type of the tensor being transformed
 *
 * @param[in] b2 The orbital space to transform mode 1 to
 * @param[in] t The tensor to transform
 *
 * @return @p t with the 1-st mode transformed according to @p b2.
 */
template<typename T1, typename T2, typename T3, typename T4,
         typename TensorType>
auto transform(AO_SPACE<T1>&, DERIVED_SPACE<T2>& b2, AO_SPACE<T3>&,
               AO_SPACE<T4>&, const TensorType& t) {
    TensorType rv;
    rv("i,m,k,l") = b2.C()("j,m") * t("i,j,k,l");
    return rv;
}

/** @brief Transforms a rank 4 tensor
 *
 *  Applies the provided transformation to the 2-nd mode of a tensor.
 *
 * @tparam T1 The scalar type of the transformation for the 0-th mode
 * @tparam T2 The scalar type of the transformation for the 1-st mode
 * @tparam T3 The scalar type of the transformation for the 2-nd mode
 * @tparam T4 The scalar type of the transformation for the 3-rd mode
 * @tparam TensorType The type of the tensor being transformed
 *
 * @param[in] k1 The orbital space to transform mode 2 to
 * @param[in] t The tensor to transform
 *
 * @return @p t with the 2-nd mode transformed according to @p k1.
 */
template<typename T1, typename T2, typename T3, typename T4,
         typename TensorType>
auto transform(AO_SPACE<T1>&, AO_SPACE<T2>&, DERIVED_SPACE<T3>& k1,
               AO_SPACE<T4>&, const TensorType& t) {
    TensorType rv;
    rv("i,j,m,l") = k1.C()("k,m") * t("i,j,k,l");
    return rv;
}

/** @brief Transforms a rank 4 tensor
 *
 *  Applies the provided transformation to the 3-rd mode of a tensor.
 *
 * @tparam T1 The scalar type of the transformation for the 0-th mode
 * @tparam T2 The scalar type of the transformation for the 1-st mode
 * @tparam T3 The scalar type of the transformation for the 2-nd mode
 * @tparam T4 The scalar type of the transformation for the 3-rd mode
 * @tparam TensorType The type of the tensor being transformed
 *
 * @param[in] k2 The orbital space to transform mode 0 to
 * @param[in] t The tensor to transform
 *
 * @return @p t with the 3-rd mode transformed according to @p k2.
 */
template<typename T1, typename T2, typename T3, typename T4,
         typename TensorType>
auto transform(AO_SPACE<T1>&, AO_SPACE<T2>&, AO_SPACE<T3>&,
               DERIVED_SPACE<T4>& k2, const TensorType& t) {
    TensorType rv;
    rv("i,j,k,m") = t("i,j,k,l") * k2.C()("l, m");
    return rv;
}

/** @brief Transforms a rank 4 tensor
 *
 *  Applies the provided transformations to the 0-th and 1-st modes of a tensor.
 *
 * @tparam T1 The scalar type of the transformation for the 0-th mode
 * @tparam T2 The scalar type of the transformation for the 1-st mode
 * @tparam T3 The scalar type of the transformation for the 2-nd mode
 * @tparam T4 The scalar type of the transformation for the 3-rd mode
 * @tparam TensorType The type of the tensor being transformed
 *
 * @param[in] b1 The orbital space to transform mode 0 to
 * @param[in] b2 The orbital space to transform mode 1 to
 * @param[in] k1 The orbital space spanning mode 2
 * @param[in] k2 The orbital space spanning mode 3
 * @param[in] t The tensor to transform
 *
 * @return @p t with the 0-th and 1-st modes transformed according to @p b1 and
 *         @p b2 respectively.
 */
template<typename T1, typename T2, typename T3, typename T4,
         typename TensorType>
auto transform(DERIVED_SPACE<T1>& b1, DERIVED_SPACE<T2>& b2, AO_SPACE<T3>& k1,
               AO_SPACE<T4>& k2, const TensorType& t) {
    const bool do_b2_first = detail_::do_which_first(b1, b2);
    if(do_b2_first) {
        auto temp = chemist::transform(b1.from_space(), b2, k1, k2, t);
        return chemist::transform(b1, b2.from_space(), k1, k2, temp);
    }
    auto temp = chemist::transform(b1, b2.from_space(), k1, k2, t);
    return chemist::transform(b1.from_space(), b2, k1, k2, temp);
}

/** @brief Transforms a rank 4 tensor
 *
 *  Applies the provided transformations to the 0-th and 2-nd modes of a tensor.
 *
 * @tparam T1 The scalar type of the transformation for the 0-th mode
 * @tparam T2 The scalar type of the transformation for the 1-st mode
 * @tparam T3 The scalar type of the transformation for the 2-nd mode
 * @tparam T4 The scalar type of the transformation for the 3-rd mode
 * @tparam TensorType The type of the tensor being transformed
 *
 * @param[in] b1 The orbital space to transform mode 0 to
 * @param[in] b2 The orbital space spanning mode 1
 * @param[in] k1 The orbital space to transform mode 2 to
 * @param[in] k2 The orbital space spanning mode 3
 * @param[in] t The tensor to transform
 *
 * @return @p t with the 0-th and 2-nd modes transformed according to @p b1 and
 *         @p k1 respectively.
 */
template<typename T1, typename T2, typename T3, typename T4,
         typename TensorType>
auto transform(DERIVED_SPACE<T1>& b1, AO_SPACE<T2>& b2, DERIVED_SPACE<T3>& k1,
               AO_SPACE<T4>& k2, const TensorType& t) {
    const bool do_k1_first = detail_::do_which_first(b1, k1);
    if(do_k1_first) {
        auto temp = chemist::transform(b1.from_space(), b2, k1, k2, t);
        return chemist::transform(b1, b2, k1.from_space(), k2, temp);
    }
    auto temp = chemist::transform(b1, b2, k1.from_space(), k2, t);
    return chemist::transform(b1.from_space(), b2, k1, k2, temp);
}

/** @brief Transforms a rank 4 tensor
 *
 *  Applies the provided transformations to the 0-th and 3-rd modes of a tensor.
 *
 * @tparam T1 The scalar type of the transformation for the 0-th mode
 * @tparam T2 The scalar type of the transformation for the 1-st mode
 * @tparam T3 The scalar type of the transformation for the 2-nd mode
 * @tparam T4 The scalar type of the transformation for the 3-rd mode
 * @tparam TensorType The type of the tensor being transformed
 *
 * @param[in] b1 The orbital space to transform mode 0 to
 * @param[in] b2 The orbital space spanning mode 1
 * @param[in] k1 The orbital space spanning mode 2
 * @param[in] k2 The orbital space to transform mode 3 to
 * @param[in] t The tensor to transform
 *
 * @return @p t with the 0-th and 3-red modes transformed according to @p b1 and
 *         @p k2 respectively.
 */
template<typename T1, typename T2, typename T3, typename T4,
         typename TensorType>
auto transform(DERIVED_SPACE<T1>& b1, AO_SPACE<T2>& b2, AO_SPACE<T3>& k1,
               DERIVED_SPACE<T4>& k2, const TensorType& t) {
    const bool do_k2_first = detail_::do_which_first(b1, k2);
    if(do_k2_first) {
        auto temp = chemist::transform(b1.from_space(), b2, k1, k2, t);
        return chemist::transform(b1, b2, k1, k2.from_space(), temp);
    }
    auto temp = chemist::transform(b1, b2, k1, k2.from_space(), t);
    return chemist::transform(b1.from_space(), b2, k1, k2, temp);
}

/** @brief Transforms a rank 4 tensor
 *
 *  Applies the provided transformations to the 1-st and 2-nd modes of a tensor.
 *
 * @tparam T1 The scalar type of the transformation for the 0-th mode
 * @tparam T2 The scalar type of the transformation for the 1-st mode
 * @tparam T3 The scalar type of the transformation for the 2-nd mode
 * @tparam T4 The scalar type of the transformation for the 3-rd mode
 * @tparam TensorType The type of the tensor being transformed
 *
 * @param[in] b1 The orbital space spanning mode 0
 * @param[in] b2 The orbital space to transform mode 1 to
 * @param[in] k1 The orbital space to transform mode 2 to
 * @param[in] k2 The orbital space spanning mode 3
 * @param[in] t The tensor to transform
 *
 * @return @p t with the 1-st and 2-nd modes transformed according to @p b2 and
 *         @p k1 respectively.
 */
template<typename T1, typename T2, typename T3, typename T4,
         typename TensorType>
auto transform(AO_SPACE<T1>& b1, DERIVED_SPACE<T2>& b2, DERIVED_SPACE<T3>& k1,
               AO_SPACE<T4>& k2, const TensorType& t) {
    const bool do_k1_first = detail_::do_which_first(b2, k1);
    if(do_k1_first) {
        auto temp = chemist::transform(b1, b2.from_space(), k1, k2, t);
        return chemist::transform(b1, b2, k1.from_space(), k2, temp);
    }
    auto temp = chemist::transform(b1, b2, k1.from_space(), k2, t);
    return chemist::transform(b1, b2.from_space(), k1, k2, temp);
}

/** @brief Transforms a rank 4 tensor
 *
 *  Applies the provided transformations to the 1-st and 3-rd modes of a tensor.
 *
 * @tparam T1 The scalar type of the transformation for the 0-th mode
 * @tparam T2 The scalar type of the transformation for the 1-st mode
 * @tparam T3 The scalar type of the transformation for the 2-nd mode
 * @tparam T4 The scalar type of the transformation for the 3-rd mode
 * @tparam TensorType The type of the tensor being transformed
 *
 * @param[in] b1 The orbital space spanning mode 0
 * @param[in] b2 The orbital space to transform mode 1 to
 * @param[in] k1 The orbital space spanning mode 2
 * @param[in] k2 The orbital space to transform mode 3 to
 * @param[in] t The tensor to transform
 *
 * @return @p t with the 1-st and 3-rd modes transformed according to @p b2 and
 *         @p k2 respectively.
 */
template<typename T1, typename T2, typename T3, typename T4,
         typename TensorType>
auto transform(AO_SPACE<T1>& b1, DERIVED_SPACE<T2>& b2, AO_SPACE<T3>& k1,
               DERIVED_SPACE<T4>& k2, const TensorType& t) {
    const bool do_k2_first = detail_::do_which_first(b2, k2);
    if(do_k2_first) {
        auto temp = chemist::transform(b1, b2.from_space(), k1, k2, t);
        return chemist::transform(b1, b2, k1, k2.from_space(), temp);
    }
    auto temp = chemist::transform(b1, b2, k1, k2.from_space(), t);
    return chemist::transform(b1, b2.from_space(), k1, k2, temp);
}

/** @brief Transforms a rank 4 tensor
 *
 *  Applies the provided transformations to the 2-nd and 3-rd modes of a tensor.
 *
 * @tparam T1 The scalar type of the transformation for the 0-th mode
 * @tparam T2 The scalar type of the transformation for the 1-st mode
 * @tparam T3 The scalar type of the transformation for the 2-nd mode
 * @tparam T4 The scalar type of the transformation for the 3-rd mode
 * @tparam TensorType The type of the tensor being transformed
 *
 * @param[in] b1 The orbital space spanning mode 0
 * @param[in] b2 The orbital space spanning mode 1
 * @param[in] k1 The orbital space to transform mode 2 to
 * @param[in] k2 The orbital space to transform mode 3 to
 * @param[in] t The tensor to transform
 *
 * @return @p t with the 2-nd and 3-nd modes transformed according to @p k1 and
 *         @p k2 respectively.
 */
template<typename T1, typename T2, typename T3, typename T4,
         typename TensorType>
auto transform(AO_SPACE<T1>& b1, AO_SPACE<T2>& b2, DERIVED_SPACE<T3>& k1,
               DERIVED_SPACE<T4>& k2, const TensorType& t) {
    const bool do_k2_first = detail_::do_which_first(k1, k2);
    if(do_k2_first) {
        auto temp = chemist::transform(b1, b2, k1.from_space(), k2, t);
        return chemist::transform(b1, b2, k1, k2.from_space(), temp);
    }
    auto temp = chemist::transform(b1, b2, k1, k2.from_space(), t);
    return chemist::transform(b1, b2, k1.from_space(), k2, temp);
}

/** @brief Transforms a rank 4 tensor
 *
 *  Applies the provided transformations to the 0-th, 1-st, and 2-nd modes of a
 *  tensor.
 *
 * @tparam T1 The scalar type of the transformation for the 0-th mode
 * @tparam T2 The scalar type of the transformation for the 1-st mode
 * @tparam T3 The scalar type of the transformation for the 2-nd mode
 * @tparam T4 The scalar type of the transformation for the 3-rd mode
 * @tparam TensorType The type of the tensor being transformed
 *
 * @param[in] b1 The orbital space to transform mode 0 to
 * @param[in] b2 The orbital space to transform mode 1 to
 * @param[in] k1 The orbital space to transform mode 2 to
 * @param[in] k2 The orbital space spanning mode 3
 * @param[in] t The tensor to transform
 *
 * @return @p t with the 0-th, 1-st, and 2-nd modes transformed according to
 *         @p b1, @p b2, and @p k1 and respectively.
 */
template<typename T1, typename T2, typename T3, typename T4,
         typename TensorType>
auto transform(DERIVED_SPACE<T1>& b1, DERIVED_SPACE<T2>& b2,
               DERIVED_SPACE<T3>& k1, AO_SPACE<T4>& k2, const TensorType& t) {
    const auto& aos0 = b1.from_space();
    const auto& aos1 = b2.from_space();
    const auto& aos2 = k1.from_space();

    switch(detail_::do_which_first(b1, b2, k1)) {
        case 0: {
            auto temp = chemist::transform(b1, aos1, aos2, k2, t);
            return chemist::transform(aos0, b2, k1, k2, temp);
        }
        case 1: {
            auto temp = chemist::transform(aos0, b2, aos2, k2, t);
            return chemist::transform(b1, aos1, k1, k2, temp);
        }
        default: {
            auto temp = chemist::transform(aos0, aos1, k1, k2, t);
            return chemist::transform(b1, b2, aos2, k2, temp);
        }
    }
}

/** @brief Transforms a rank 4 tensor
 *
 *  Applies the provided transformations to the 0-th, 1-st, and 3-rd modes of a
 *  tensor.
 *
 * @tparam T1 The scalar type of the transformation for the 0-th mode
 * @tparam T2 The scalar type of the transformation for the 1-st mode
 * @tparam T3 The scalar type of the transformation for the 2-nd mode
 * @tparam T4 The scalar type of the transformation for the 3-rd mode
 * @tparam TensorType The type of the tensor being transformed
 *
 * @param[in] b1 The orbital space to transform mode 0 to
 * @param[in] b2 The orbital space to transform mode 1 to
 * @param[in] k1 The orbital space spanning mode 2
 * @param[in] k2 The orbital space to transform mode 3 to
 * @param[in] t The tensor to transform
 *
 * @return @p t with the 0-th, 1-st, and 3-rd modes transformed according to
 *         @p b1, @p b2, and @p k2 and respectively.
 */
template<typename T1, typename T2, typename T3, typename T4,
         typename TensorType>
auto transform(DERIVED_SPACE<T1>& b1, DERIVED_SPACE<T2>& b2, AO_SPACE<T3>& k1,
               DERIVED_SPACE<T4>& k2, const TensorType& t) {
    const auto& aos0 = b1.from_space();
    const auto& aos1 = b2.from_space();
    const auto& aos3 = k2.from_space();

    switch(detail_::do_which_first(b1, b2, k2)) {
        case 0: {
            auto temp = chemist::transform(b1, aos1, k1, aos3, t);
            return chemist::transform(aos0, b2, k1, k2, temp);
        }
        case 1: {
            auto temp = chemist::transform(aos0, b2, k1, aos3, t);
            return chemist::transform(b1, aos1, k1, k2, temp);
        }
        default: {
            auto temp = chemist::transform(aos0, aos1, k1, k2, t);
            return chemist::transform(b1, b2, k1, aos3, temp);
        }
    }
}

/** @brief Transforms a rank 4 tensor
 *
 *  Applies the provided transformations to the 0-th, 2-nd, and 3-rd modes of a
 *  tensor.
 *
 * @tparam T1 The scalar type of the transformation for the 0-th mode
 * @tparam T2 The scalar type of the transformation for the 1-st mode
 * @tparam T3 The scalar type of the transformation for the 2-nd mode
 * @tparam T4 The scalar type of the transformation for the 3-rd mode
 * @tparam TensorType The type of the tensor being transformed
 *
 * @param[in] b1 The orbital space to transform mode 0 to
 * @param[in] b2 The orbital space spanning mode 1
 * @param[in] k1 The orbital space to transform mode 2 to
 * @param[in] k2 The orbital space to transform mode 3 to
 * @param[in] t The tensor to transform
 *
 * @return @p t with the 0-th, 2-nd, and 3-rd modes transformed according to
 *         @p b1, @p k1, and @p k2 and respectively.
 */
template<typename T1, typename T2, typename T3, typename T4,
         typename TensorType>
auto transform(DERIVED_SPACE<T1>& b1, AO_SPACE<T2>& b2, DERIVED_SPACE<T3>& k1,
               DERIVED_SPACE<T4>& k2, const TensorType& t) {
    const auto& aos0 = b1.from_space();
    const auto& aos2 = k1.from_space();
    const auto& aos3 = k2.from_space();

    switch(detail_::do_which_first(b1, k1, k2)) {
        case 0: {
            auto temp = chemist::transform(b1, b2, aos2, aos3, t);
            return chemist::transform(aos0, b2, k1, k2, temp);
        }
        case 1: {
            auto temp = chemist::transform(aos0, b2, k1, aos3, t);
            return chemist::transform(b1, b2, aos2, k2, temp);
        }
        default: {
            auto temp = chemist::transform(aos0, b2, aos2, k2, t);
            return chemist::transform(b1, b2, k1, aos3, temp);
        }
    }
}

/** @brief Transforms a rank 4 tensor
 *
 *  Applies the provided transformations to the 1-st, 2-nd, and 3-rd modes of a
 *  tensor.
 *
 * @tparam T1 The scalar type of the transformation for the 0-th mode
 * @tparam T2 The scalar type of the transformation for the 1-st mode
 * @tparam T3 The scalar type of the transformation for the 2-nd mode
 * @tparam T4 The scalar type of the transformation for the 3-rd mode
 * @tparam TensorType The type of the tensor being transformed
 *
 * @param[in] b1 The orbital space spanning mode 0
 * @param[in] b2 The orbital space to transform mode 1 to
 * @param[in] k1 The orbital space to transform mode 2 to
 * @param[in] k2 The orbital space to transform mode 3 to
 * @param[in] t The tensor to transform
 *
 * @return @p t with the 1-st, 2-nd, and 3-rd modes transformed according to
 *         @p b2, @p k1, and @p k2 and respectively.
 */
template<typename T1, typename T2, typename T3, typename T4,
         typename TensorType>
auto transform(AO_SPACE<T1>& b1, DERIVED_SPACE<T2>& b2, DERIVED_SPACE<T3>& k1,
               DERIVED_SPACE<T4>& k2, const TensorType& t) {
    const auto& aos1 = b2.from_space();
    const auto& aos2 = k1.from_space();
    const auto& aos3 = k2.from_space();

    switch(detail_::do_which_first(b2, k1, k2)) {
        case 0: {
            auto temp = chemist::transform(b1, b2, aos2, aos3, t);
            return chemist::transform(b1, aos1, k1, k2, temp);
        }
        case 1: {
            auto temp = chemist::transform(b1, aos1, k1, aos3, t);
            return chemist::transform(b1, b2, aos2, k2, temp);
        }
        default: {
            auto temp = chemist::transform(b1, aos1, aos2, k2, t);
            return chemist::transform(b1, b2, k1, aos3, temp);
        }
    }
}

/** @brief Transforms a rank 4 tensor
 *
 *  Applies the provided transformations to the 0-th, 1-st, 2-nd, and 3-rd modes
 *  of a tensor.
 *
 * @tparam T1 The scalar type of the transformation for the 0-th mode
 * @tparam T2 The scalar type of the transformation for the 1-st mode
 * @tparam T3 The scalar type of the transformation for the 2-nd mode
 * @tparam T4 The scalar type of the transformation for the 3-rd mode
 * @tparam TensorType The type of the tensor being transformed
 *
 * @param[in] b1 The orbital space to transform mode 0 to
 * @param[in] b2 The orbital space to transform mode 1 to
 * @param[in] k1 The orbital space to transform mode 2 to
 * @param[in] k2 The orbital space to transform mode 3 to
 * @param[in] t The tensor to transform
 *
 * @return @p t with the 0-th, 1-st, 2-nd, and 3-rd modes transformed according
 *         to @p b1, @p b2, @p k1, and @p k2 and respectively.
 */
template<typename T1, typename T2, typename T3, typename T4,
         typename TensorType>
auto transform(DERIVED_SPACE<T1>& b1, DERIVED_SPACE<T2>& b2,
               DERIVED_SPACE<T3>& k1, DERIVED_SPACE<T4>& k2,
               const TensorType& t) {
    const auto& aos0 = b1.from_space();
    const auto& aos1 = b2.from_space();
    const auto& aos2 = k1.from_space();
    const auto& aos3 = k2.from_space();

    switch(detail_::do_which_first(b1, b2, k1, k2)) {
        case 0: {
            auto temp = chemist::transform(b1, aos1, aos2, aos3, t);
            return chemist::transform(aos0, b2, k1, k2, temp);
        }
        case 1: {
            auto temp = chemist::transform(aos0, b2, aos2, aos3, t);
            return chemist::transform(b1, aos1, k1, k2, temp);
        }
        case 2: {
            auto temp = chemist::transform(aos0, aos1, k1, aos3, t);
            return chemist::transform(b1, b2, aos2, k2, temp);
        }
        default: {
            auto temp = chemist::transform(aos0, aos1, aos2, k2, t);
            return chemist::transform(b1, b2, k1, aos3, temp);
        }
    }
}

#undef DERIVED_SPACE
#undef AO_SPACE
} // namespace chemist
