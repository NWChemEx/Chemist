#pragma once
#include "chemist/sparse_map/sparse_map/sparse_map.hpp"
#include "chemist/ta_helpers/ta_hashers.hpp"
#include "chemist/ta_helpers/ta_helpers.hpp"
#include "chemist/types.hpp"

/* Testing notes:
 *
 * - We assume that DependentBaseSpace_ does not ensure compatability between
 *   the sparse map instance and the overlap matrix. If it does then these tests
 *   will need updated so that the sparse maps are consistent with the overlap
 *   matrices.
 */

namespace chemist::test {

template<typename T>
struct TensorMaker {
    static auto S(TA::World& world) {
        return T(world, {{1.0, 0.5}, {0.5, 1.0}});
    }

    static auto S2(TA::World& world) {
        return T(world, {{1.0, 0.25}, {0.25, 1.0}});
    }

    static auto corr_transformed_S(TA::World& world) {
        return T(world, {{1.3125, 1.03125}, {1.03125, 1.3125}});
    }

    static auto corr_rho(TA::World& world) {
        return T(world, {{1.0625, 0.5}, {0.5, 1.0625}});
    }
};

template<typename T>
struct TensorMaker<type::tensor_of_tensors<T>> {
    using tensor_type = typename type::tensor_of_tensors<T>;
    using tile_type   = typename tensor_type::value_type;
    using inner_type  = typename tile_type::value_type;

    static auto S(TA::World& world) {
        inner_type t1(TA::Range(2, 2), {1.0, 0.5, 0.5, 1.0});
        inner_type t2(TA::Range(2, 3), {0.12, 0.23, 0.34, 0.45, 0.56, 0.67});
        inner_type t3(TA::Range(3, 2), {0.12, 0.45, 0.23, 0.56, 0.34, 0.67});
        inner_type t4(TA::Range(3,3), {1.0, 0.12, 0.23, 0.12, 1.0, 0.34, 0.23,
                                        0.34, 1.0});
        return tensor_type(world, {{t1, t2}, {t3, t4}});
    }

    static auto S2(TA::World& world) {
        inner_type t(TA::Range(2, 2), {1.0, 0.25, 0.25, 1.0});
        inner_type t2(TA::Range(3,3), {1.0, 0.45, 0.56, 0.45, 1.0, 0.67, 0.56,
                                       0.67, 1.0});
        return tensor_type(world, {t, t2});
    }

    static auto corr_transformed_S(TA::World& world) {
        inner_type t(TA::Range(2, 2), {1.3125, 1.03125, 1.03125, 1.3125});
        inner_type t2(TA::Range(2, 3), {0.6832, 0.81465, 0.8856, 1.180675,
                                        1.33935,  1.437525});
        inner_type t3(TA::Range(3,2), {0.6832, 1.180675, 0.81465, 1.33935,
                                       0.8856, 1.437525});
        inner_type t4(TA::Range(3,3), {2.05306, 1.92447, 2.114836, 1.92447,
                                       2.35369, 2.377802, 2.114836, 2.377802,
                                       2.565748});
        return tensor_type(world, {{t, t2}, {t3, t4}});
    }

    // placeholder until density() works for ToT case
    static auto corr_rho(TA::World& world) {
        tensor_type rv;
        return rv;
    }
};

using index_type = sparse_map::ElementIndex;
static sparse_map::SparseMap<index_type, index_type> sm1;
static sparse_map::SparseMap<index_type, index_type> sm2({{index_type{0ul}, {index_type{0, 1}}}});

template<typename T, typename U>
struct CompareTensors{
    static bool allclose(const T& lhs, const U& rhs) {
        return chemist::ta_helpers::allclose(lhs, rhs);
    }
};

template<typename T, typename U>
struct CompareTensors<type::tensor_of_tensors<T>, type::tensor_of_tensors<U>> {
    using lhs_type = type::tensor_of_tensors<T>;
    using rhs_type = type::tensor_of_tensors<U>;
    static bool allclose(const lhs_type& lhs, const rhs_type& rhs) {
        return chemist::ta_helpers::allclose_tot(lhs, rhs, 2);
    }
};

template<typename T, typename U>
bool compare_tensors(T&& lhs, U&& rhs) {
    return CompareTensors<std::decay_t<T>, std::decay_t<U>>::allclose(lhs, rhs);
}

}
