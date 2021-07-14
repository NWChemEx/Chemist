#include "libchemist/ta_helpers/ta_hashers.hpp"
#include "libchemist/ta_helpers/ta_helpers.hpp"
#include <catch2/catch.hpp>
#include <sde/hasher.hpp>

using sde::hash_objects;
/* Alternative to sde::hash_objects
auto hash_string = [](auto a) {
    return sde::hash_to_string(
        sde::make_hash(sde::HashType::Hash128, a));
};
*/

int get_largest_factor(const int n) {
    int largest_factor = 1;
    for(unsigned i = 1; i <= n / 2; ++i)
        if(n % i == 0) largest_factor = i;
    return largest_factor;
}

TEST_CASE("TA Hashers", "[TA],[hash]") {
    auto& world = TA::get_default_world();
    // NOTE: hashing Range R0() fails, requires R0({})
    TA::Range R0({});
    TA::Range R1({0, 0}, {10, 10});
    TA::Range R2({0, 0}, {10, 10});
    TA::Range R3({0, 0}, {5, 5});
    TA::Range R4({5, 10, 20}, {10, 20, 30});
    TA::TiledRange1 tr0{};
    TA::TiledRange1 tr1{0, 10, 20};
    TA::TiledRange1 tr2{0, 10, 20};
    TA::TiledRange1 tr3{0, 10, 20, 30};
    TA::TiledRange1 tr4{10, 20, 30};
    TA::TiledRange TR0{};
    TA::TiledRange TR1{tr1, tr1};
    TA::TiledRange TR2{{0, 10, 20}, {0, 10, 20}};
    TA::TiledRange TR3{tr3, tr3};
    TA::TiledRange TR4{tr4, tr4};

    SECTION("TA::Range Relative hash tests") {
        REQUIRE(hash_objects(R0) == hash_objects(R0));
        REQUIRE(hash_objects(R0) != hash_objects(R1));
        REQUIRE(hash_objects(R1) == hash_objects(R1));
        REQUIRE(hash_objects(R1) == hash_objects(R2));
        REQUIRE(hash_objects(R1) != hash_objects(R3));
        REQUIRE(hash_objects(R3) != hash_objects(R4));
    }
    // These tests strictly depend on the hash implementation
    SECTION("TA::Range absolute hash tests") {
        CHECK(hash_objects(R0) == "ebeb4a60f6934d9bbbeb46fe4804a84d");
        CHECK(hash_objects(R1) == "a12daf2b1bf55784c818a6ad47856bcd");
    }
    SECTION("TA::TiledRange relative hash tests") {
        REQUIRE(hash_objects(tr0) == hash_objects(tr0));
        REQUIRE(hash_objects(tr0) != hash_objects(tr1));
        REQUIRE(hash_objects(tr1) == hash_objects(tr1));
        REQUIRE(hash_objects(tr1) == hash_objects(tr2));
        REQUIRE(hash_objects(tr1) != hash_objects(tr3));
        REQUIRE(hash_objects(tr2) != hash_objects(tr3));
        REQUIRE(hash_objects(tr3) != hash_objects(tr4));
        REQUIRE(hash_objects(tr0) != hash_objects(TR0));
        REQUIRE(hash_objects(TR0) != hash_objects(TR1));
        REQUIRE(hash_objects(TR1) == hash_objects(TR2));
        REQUIRE(hash_objects(TR2) != hash_objects(TR3));
        REQUIRE(hash_objects(TR3) != hash_objects(TR4));
        REQUIRE(hash_objects(tr1) != hash_objects(TR3));
    }
    SECTION("TA::TiledRange absolute hash tests") {
        CHECK(hash_objects(tr0) == "fde49c1aa57ad98195cfc08c3e549ae1");
        CHECK(hash_objects(tr1) == "b6b6ff66214fe06355979d65b72736bb");
        CHECK(hash_objects(TR0) == "94d6d17d9a96268c42263124f58c754f");
        CHECK(hash_objects(TR1) == "bf931e548ad5a8192f10724969e80cc9");
    }
    TA::TArrayD TAD1(world, TR1);
    TA::TArrayF TAF1(world, TR1);
    TA::TArrayI TAI1(world, TR1);
    TA::TArrayL TAL1(world, TR1);
    TA::TArrayD TA2(world, TR1);
    TA::TArrayD TA3(world, TR2);
    TA::TArrayD TA4(world, TR2);
    TA::TArrayD TA5(world, TR3);
    TA::TArrayD TA6(world, TR4);
    // Note: If you don't fill the array, it hangs during hashing.
    TAD1.fill(1);
    TAF1.fill(1);
    TAI1.fill(1);
    TAL1.fill(1);
    TA2.fill(1.0);
    TA3.fill(3.0);
    TA4.fill(10.0);
    TA5.fill(14.0);
    TA6.fill(1.0);
    TA::TensorD T1(TR1.tiles_range(), 0.0);
    TA::TensorD T2(TR1.tiles_range(), 0.0);
    TA::TensorD T3(TR1.tiles_range(), 1.0);
    TA::TensorD T4(TR3.tiles_range(), 0.0);
    TA::TensorD T5 = TAD1.find({0, 0}).get();
    TA::TensorD T6 = TAD1.find({0, 0}).get();
    TA::TensorD T7 = TA3.find({0, 0}).get();
    TA::TSpArrayD SA1(world, {{1.0, 2.0}, {3.0, 4.0}});
    TA::TSpArrayD SA2(world, TR1);
    TA::TSpArrayD SA3(world, TR1);
    TA::TSpArrayD SA4(world, TR3);
    SA2.fill(1.0);
    SA3.fill(2.0);
    SA4.fill(1.0);
    SECTION("TA:DistArray and TA::Tensor relative hash tests") {
        REQUIRE(hash_objects(TAD1) == hash_objects(TAD1));
        REQUIRE(hash_objects(TAD1) != hash_objects(TAF1));
        REQUIRE(hash_objects(TAF1) != hash_objects(TAI1));
        REQUIRE(hash_objects(TAI1) != hash_objects(TAL1));
        REQUIRE(hash_objects(TAD1) == hash_objects(TA2));
        REQUIRE(hash_objects(TA2) != hash_objects(TA3));
        REQUIRE(hash_objects(TA3) != hash_objects(TA4));
        REQUIRE(hash_objects(TA3) != hash_objects(TA4));
        REQUIRE(hash_objects(TA4) != hash_objects(TA5));
        REQUIRE(hash_objects(TA3) != hash_objects(TA4));
        REQUIRE(hash_objects(TA3) != hash_objects(TA4));
        REQUIRE(hash_objects(SA1) == hash_objects(SA1));
        REQUIRE(hash_objects(SA2) != hash_objects(SA3));
        REQUIRE(hash_objects(SA2) != hash_objects(SA4));
        REQUIRE(hash_objects(SA3) != hash_objects(SA4));
    }

    SECTION("Direct TA:DistArray and TA::Tensor comparisons") {
        REQUIRE(TAD1 == TAD1);
        REQUIRE(TAD1 != TAF1);
        REQUIRE(TAI1 != TAL1);
        REQUIRE(TAD1 == TA2);
        REQUIRE(TA2 != TA3);
        REQUIRE(TA3 != TA4);
        REQUIRE(SA2 != SA3);
    }

    SECTION("TA::DistArray and TA::Tensor absolute hash tests") {
        CHECK(hash_objects(TAD1) == "664c65f3cb54d763a1c273ad2f41d523");
        CHECK(hash_objects(TA4) == "2b37ca4c5071ef24a64f67e911485699");
        CHECK(hash_objects(TA5) == "0bc0e4449c08461ab99e0fbed0bdb44d");
        CHECK(hash_objects(T1) == "95b4179d186d2b0e4afeb3cda95a4a26");
        CHECK(hash_objects(T3) == "eda495a3050cff2974a762ec3a51d846");
        CHECK(hash_objects(T4) == "3a95aebea8dfe5db41302f6d5c5098f1");
        CHECK(hash_objects(T5) == "a5a2cbcce9d9e8f03b5cdfefba138ada");
        CHECK(hash_objects(SA1) == "7349ed990d4aad3ad68777480836abb5");
    }
}

TEMPLATE_TEST_CASE("TA hashing templated tests for tensors",
                   "[hash][TA][tensor]", int, long, float, double) {
    auto& world = TA::get_default_world();

    TA::TiledRange TR1{{0, 10, 20}, {0, 10, 20}};
    TA::TiledRange TR2{{0, 5, 10}, {0, 5, 10}};
    TA::TiledRange TR3{{0, 1, 2}, {0, 15, 30, 40}};
    TA::TiledRange TR4{{0, 1, 2}, {0, 1, 2}};
    int worldsizefactor  = get_largest_factor(world.size());
    int tilesizefactor   = get_largest_factor(TR3.tiles_range().volume());
    auto replicated_pmap = std::make_shared<TA::detail::ReplicatedPmap>(
      world, TR3.tiles_range().volume());
    auto blocked_pmap = std::make_shared<TA::detail::BlockedPmap>(
      world, TR3.tiles_range().volume());
    auto cyclic_pmap = std::make_shared<TA::detail::CyclicPmap>(
      world, tilesizefactor, TR3.tiles_range().volume() / tilesizefactor,
      worldsizefactor, world.size() / worldsizefactor);

    TA::TArray<TestType> A0(world, TR1);
    TA::TArray<TestType> A1(world, TR1);
    TA::TArray<TestType> A2(world, TR1);
    TA::TArray<TestType> A3(world, TR2);
    TA::TArray<TestType> A4(world, TR2);
    TA::TArray<TestType> A5(world, TR3, blocked_pmap);
    TA::TArray<TestType> A5r(world, TR3, replicated_pmap);
    TA::TArray<TestType> A5c(world, TR3, cyclic_pmap);

    A0.fill(0); // If you omit filling, test hangs
    A1.fill(1);
    A2.fill(1);
    A3.fill(1);
    A4.fill(2);
    A5.fill(3);
    A5r.fill(3);
    A5c.fill(3);

    TA::Tensor<TestType, Eigen::aligned_allocator<TestType>> T1(
      TR1.tiles_range(), 0.0);
    TA::Tensor<TestType, Eigen::aligned_allocator<TestType>> T2(
      TR1.tiles_range(), 0.0);
    TA::Tensor<TestType, Eigen::aligned_allocator<TestType>> T3(
      TR1.tiles_range(), 1.0);
    TA::Tensor<TestType, Eigen::aligned_allocator<TestType>> T4(
      TR3.tiles_range(), 0.0);
    TA::Tensor<TestType, Eigen::aligned_allocator<TestType>> T5 =
      A1.find({0, 0}).get();
    TA::Tensor<TestType, Eigen::aligned_allocator<TestType>> T6 =
      A2.find({0, 0}).get();
    TA::Tensor<TestType, Eigen::aligned_allocator<TestType>> T7 =
      A3.find({0, 0}).get();
    TA::TSpArray<TestType> SA0(world, TR1);
    TA::TSpArray<TestType> SA1(world, TR1);
    TA::TSpArray<TestType> SA2(world, TR2);
    TA::TSpArray<TestType> SA3(world, TR3, blocked_pmap);
    TA::TSpArray<TestType> SA3r(world, TR3, replicated_pmap);
    TA::TSpArray<TestType> SA3c(world, TR3, cyclic_pmap);

    SA0.fill(0);
    SA1.fill(0);
    SA2.fill(0);
    SA3.fill(1);
    SA3r.fill(1);
    SA3c.fill(1);

    SECTION("Hash for uninitialized TA objects") {
        TA::TArray<TestType> UA1;
        TA::TArray<TestType> UA2;
        TA::TSpArray<TestType> US1;
        TA::TSpArray<TestType> US2;
        REQUIRE(hash_objects(UA1) == hash_objects(UA2));
        REQUIRE(hash_objects(US1) == hash_objects(US2));
    }

    SECTION("TA::DistArray and TA::Tensor relative hash tests") {
        REQUIRE(hash_objects(A0) == hash_objects(A0));
        REQUIRE(hash_objects(A0) != hash_objects(A1));
        REQUIRE(hash_objects(A1) == hash_objects(A2));
        REQUIRE(hash_objects(A1) != hash_objects(A3));
        REQUIRE(hash_objects(A3) != hash_objects(A4));
        REQUIRE(hash_objects(T1) == hash_objects(T1));
        REQUIRE(hash_objects(T1) == hash_objects(T2));
        REQUIRE(hash_objects(T2) != hash_objects(T3));
        REQUIRE(hash_objects(T2) != hash_objects(T4));
        REQUIRE(hash_objects(T3) != hash_objects(T4));
        REQUIRE(hash_objects(T5) == hash_objects(T6));
        REQUIRE(hash_objects(T6) != hash_objects(T7));
        REQUIRE(hash_objects(SA0) == hash_objects(SA0));
        REQUIRE(hash_objects(SA0) == hash_objects(SA1));
        REQUIRE(hash_objects(SA1) != hash_objects(SA2));
        REQUIRE(hash_objects(SA2) != hash_objects(SA3));
    }

    SECTION("TA::DistArray hashes with blocked, replicated and cyclic pmap") {
        REQUIRE(hash_objects(A5) == hash_objects(A5r));
        REQUIRE(hash_objects(A5) == hash_objects(A5c));
        REQUIRE(hash_objects(SA3) == hash_objects(SA3r));
        REQUIRE(hash_objects(SA3) == hash_objects(SA3c));
    }

    SECTION("Direct TA:DistArray and TA::Tensor comparisons") {
        REQUIRE(A0 != A1);
        REQUIRE(A1 == A2);
        REQUIRE(A3 != A4);
        REQUIRE(T1 == T2);
        REQUIRE(T3 != T4);
        REQUIRE(T5 == T6);
        REQUIRE(T6 != T7);
        REQUIRE(SA0 == SA1);
        REQUIRE(SA2 != SA3);
    }
}
