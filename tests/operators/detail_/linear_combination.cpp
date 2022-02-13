#include "chemist/operators/coulomb.hpp"
#include "chemist/operators/fock.hpp"
#include "chemist/operators/hamiltonian/hamiltonian.hpp"
#include <catch2/catch.hpp>
#include <iostream>

using namespace chemist;
using namespace chemist::operators;
using namespace chemist::operators::detail_;

inline constexpr const auto& as_derived_operator(const LinearCombination& op) {
    return op;
}

TEST_CASE("LinearCombination") {
    using chemist::Atom;
    chemist::Nuclei a{Atom(1ul, std::array<double, 3>{0.0, 0.0, 0.0})};
    chemist::Nuclei b{Atom(2ul, std::array<double, 3>{0.0, 0.0, 0.0})};

    ElectronNuclearAttraction V1(Electron{}, a);
    ElectronNuclearAttraction V2(Electron{}, b);

    Hamiltonian h_default, h_v1(V1), h_v2(V2);
    Fock f_default, f_v1(V1), f_v2(V2);

    SECTION("Default Comparisons: Nonpolymorphic") {
        Hamiltonian h_compare;
        REQUIRE(h_default == h_compare);
    }

    SECTION("Stateful Comparisons: Nonpolymorphic") {
        Hamiltonian h_cpy(h_v1);
        REQUIRE(h_v1 == h_cpy);
        REQUIRE(h_v1 != h_v2);
    }

    SECTION("Default + Stateful Comparisons: Nonpolymorphic") {
        REQUIRE(h_default != h_v1);
    }

    SECTION("Default Comparisons: Polymorphic") {
        auto& d_h = as_derived_operator(h_default);
        auto& d_f = as_derived_operator(f_default);

        REQUIRE(d_h == d_f); // Nonpolymorphic comparison

        REQUIRE(h_default.is_equal(d_h));
        REQUIRE(d_h.is_equal(h_default));
        REQUIRE(not f_default.is_equal(d_h));
        REQUIRE(not d_h.is_equal(f_default));
        REQUIRE(not d_h.is_equal(d_f));
        REQUIRE(not h_default.is_equal(d_f));
        REQUIRE(not d_f.is_equal(h_default));
        REQUIRE(not d_f.is_equal(d_h));
    }

    SECTION("Stateful Comparisons: Polymorhpic") {
        auto& d_h1 = as_derived_operator(h_v1);
        auto& d_h2 = as_derived_operator(h_v2);
        auto& d_f1 = as_derived_operator(f_v1);
        auto& d_f2 = as_derived_operator(f_v2);

        REQUIRE(d_h1 == d_f1);
        REQUIRE(d_h2 == d_f2);
        REQUIRE(d_h1 != d_f2);
        REQUIRE(d_h2 != d_f1);

        REQUIRE(h_v1.is_equal(d_h1));
        REQUIRE(d_h1.is_equal(h_v1));

        REQUIRE(not f_v1.is_equal(d_h1));
        REQUIRE(not d_f1.is_equal(h_v1));
        REQUIRE(not h_v1.is_equal(d_f1));
        REQUIRE(not d_h1.is_equal(f_v1));
    }

    SECTION("hash") {
        using chemist::detail_::hash_objects;

        auto h_hash  = hash_objects(h_default);
        auto f_hash  = hash_objects(f_default);
        auto h1_hash = hash_objects(h_v1);
        auto f1_hash = hash_objects(f_v1);

        auto& d_h  = as_derived_operator(h_default);
        auto& d_f  = as_derived_operator(f_default);
        auto& d_h1 = as_derived_operator(h_v1);
        auto& d_f1 = as_derived_operator(f_v1);

        auto d_h_hash  = hash_objects(d_h);
        auto d_f_hash  = hash_objects(d_f);
        auto d_h1_hash = hash_objects(d_h1);
        auto d_f1_hash = hash_objects(d_f1);

        REQUIRE(h_hash == hash_objects(Hamiltonian{}));
        REQUIRE(f_hash == hash_objects(Fock{}));
        REQUIRE(h1_hash == hash_objects(Hamiltonian{V1}));
        REQUIRE(f1_hash == hash_objects(Fock{V1}));
#ifdef BPHASH_USE_TYPEID
        REQUIRE_FALSE(h_hash == f_hash);
        REQUIRE_FALSE(h1_hash == f1_hash);
        REQUIRE_FALSE(d_h_hash == h_hash);
        REQUIRE_FALSE(d_f_hash == f_hash);
        REQUIRE_FALSE(d_h1_hash == h1_hash);
        REQUIRE_FALSE(d_f1_hash == f1_hash);
#else
        REQUIRE(h_hash == f_hash);
        REQUIRE(h1_hash == f1_hash);
        REQUIRE(d_h_hash == h_hash);
        REQUIRE(d_f_hash == f_hash);
        REQUIRE(d_h1_hash == h1_hash);
        REQUIRE(d_f1_hash == f1_hash);
#endif

        REQUIRE_FALSE(h_hash == h1_hash);
    }
}
