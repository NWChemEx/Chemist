#pragma once
#include "libchemist/sparse_map/sparse_map.hpp"
#include "libchemist/ta_helpers/ta_hashers.hpp"
#include "libchemist/ta_helpers/ta_helpers.hpp"
#include "libchemist/types.hpp"

/* Testing notes:
 *
 * - We assume that DependentBaseSpace_ does not ensure compatability between
 *   the sparse map instance and the overlap matrix. If it does then these tests
 *   will need updated so that the sparse maps are consistent with the overlap
 *   matrices.
 */

namespace libchemist::test {

template<typename T>
struct TensorMaker {
    static auto S(TA::World& world) {
        return T(world, {1.0, 0.5, 0.5, 1.0});
    }

    static auto S2(TA::World& world) {
        return T(world, {1.0, 0.25, 0.25, 1.0});
    }
};

template<typename T>
struct TensorMaker<type::tensor_of_tensors<T>> {
    using tensor_type = typename type::tensor_of_tensors<T>;
    using tile_type   = typename tensor_type::value_type;
    using inner_type  = typename tile_type::value_type;

    static auto S(TA::World& world) {
        inner_type t(TA::Range(2, 2), {1.0, 0.5, 0.5, 1.0});
        return tensor_type(world, {t});
    }

    static auto S2(TA::World& world) {
        inner_type t(TA::Range(2, 2), {1.0, 0.25, 0.25, 1.0});
        return tensor_type(world, {t});
    }
};

using index_type = typename sparse_map::SparseMap::key_type;
static sparse_map::SparseMap sm1;
static sparse_map::SparseMap sm2({{index_type{0ul}, {index_type{0, 1}}}});

template<typename T, typename U>
struct CompareTensors{
    static bool allclose(const T& lhs, const U& rhs) {
        return libchemist::allclose(lhs, rhs);
    }
};

template<typename T, typename U>
struct CompareTensors<type::tensor_of_tensors<T>, type::tensor_of_tensors<U>> {
    using lhs_type = type::tensor_of_tensors<T>;
    using rhs_type = type::tensor_of_tensors<U>;
    static bool allclose(const lhs_type& lhs, const rhs_type& rhs) {
        return libchemist::allclose_tot(lhs, rhs, 2);
    }
};

template<typename T, typename U>
bool compare_tensors(T&& lhs, U&& rhs) {
    return CompareTensors<std::decay_t<T>, std::decay_t<U>>::allclose(lhs, rhs);
}

}
