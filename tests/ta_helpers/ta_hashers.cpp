#include <catch2/catch.hpp>
#include <libchemist/ta_helpers/ta_hashers.hpp>

TEST_CASE("TA Hashers with BPHASH", "[TA],[hash]") {
    auto& world      = TA::get_default_world();
    auto hash_string = [](auto a) {
        return bphash::hash_to_string(
          bphash::make_hash(bphash::HashType::Hash128, a));
    };
    SECTION("TA::Range") {
        // NOTE: hashing Range R0() fails, requires R0({})
        TA::Range R0({});
        TA::Range R1({0, 0}, {10, 10});
        TA::Range R2({0, 0}, {10, 10});
        TA::Range R3({0, 0}, {5, 5});
        TA::Range R4({5, 10, 20}, {10, 20, 30});
        SECTION("Relative tests") {
            REQUIRE(hash_string(R0) == hash_string(R0));
            REQUIRE(hash_string(R0) != hash_string(R1));
            REQUIRE(hash_string(R1) == hash_string(R1));
            REQUIRE(hash_string(R1) == hash_string(R2));
            REQUIRE(hash_string(R1) != hash_string(R3));
            REQUIRE(hash_string(R3) != hash_string(R4));
        }
        // These tests strictly depend on hash implementation
        SECTION("Absolute tests") {
            CHECK(hash_string(R0) == "ebeb4a60f6934d9bbbeb46fe4804a84d");
            CHECK(hash_string(R1) == "a12daf2b1bf55784c818a6ad47856bcd");
            CHECK(hash_string(R3) == "c2a4d10163c903bca9bcda0b5d0a110c");
            CHECK(hash_string(R4) == "3188973e7814661832a815b06b50b22c");
        }
    }
    SECTION("TA::TiledRange1 and TA::TiledRange hash:") {
        TA::TiledRange1 tr0{};
        TA::TiledRange1 tr1{0, 10, 20};
        TA::TiledRange1 tr2{0, 10, 20};
        TA::TiledRange1 tr3{0, 10, 20, 30};
        TA::TiledRange TR0{};
        TA::TiledRange TR1{tr1, tr1};
        TA::TiledRange TR2{{0, 10, 20}, {0, 10, 20}};
        TA::TiledRange TR3{tr3, tr3};
        SECTION("Relative tests") {
            REQUIRE(hash_string(tr0) == hash_string(tr0));
            REQUIRE(hash_string(tr0) != hash_string(tr1));
            REQUIRE(hash_string(tr1) == hash_string(tr1));
            REQUIRE(hash_string(tr1) == hash_string(tr2));
            REQUIRE(hash_string(tr1) != hash_string(tr3));
            REQUIRE(hash_string(tr0) != hash_string(TR0));
            REQUIRE(hash_string(TR0) != hash_string(TR1));
            REQUIRE(hash_string(TR1) == hash_string(TR2));
            REQUIRE(hash_string(tr1) != hash_string(TR3));
        }
        SECTION("Absolute hash tests") {
            CHECK(hash_string(tr0) == "fde49c1aa57ad98195cfc08c3e549ae1");
            CHECK(hash_string(tr1) == "b6b6ff66214fe06355979d65b72736bb");
            CHECK(hash_string(tr3) == "c700de70f48345cf37f5bec3fc4078e7");
            CHECK(hash_string(TR0) == "94d6d17d9a96268c42263124f58c754f");
            CHECK(hash_string(TR1) == "bf931e548ad5a8192f10724969e80cc9");
            CHECK(hash_string(TR3) == "1aaa2a5212f8e1c2679132a3543a82a1");
        }
    }
    SECTION("TA::DistArray and TA::Tensor absolute hash tests") {
        TA::TiledRange TR1{{0, 10, 20}, {0, 10, 20}};
        TA::TiledRange TR2{{0, 5, 10}, {0, 5, 10}};
        TA::TiledRange TR3{{0, 15, 30, 40}, {0, 15, 30, 40}};
        TA::TArrayD A1(world, TR1);
        TA::TArrayD A2(world, TR1);
        TA::TArrayD A3(world, TR2);
        TA::TArrayD A4(world, TR2);
        TA::TArrayD A5(world, TR3);
        A1.fill(1.0);
        A2.fill(1.0);
        A3.fill(1.0);
        A4.fill(10.0);
        A5.fill(14.0);
        TA::TensorD T1(TR1.tiles_range(), 0.0);
        TA::TensorD T2(TR1.tiles_range(), 0.0);
        TA::TensorD T3(TR1.tiles_range(), 1.0);
        TA::TensorD T4(TR3.tiles_range(), 0.0);
        TA::TensorD T5 = A1.find({0, 0}).get();
        TA::TensorD T6 = A1.find({0, 0}).get();
        TA::TensorD T7 = A3.find({0, 0}).get();
        TA::TSpArrayD SA1(world, {{1.0, 2.0}, {3.0, 4.0}});
        TA::TSpArrayD SA2(world, {{10.0, 20.0}, {30.0, 40.0}});
        TA::TSpArrayD SA3(world, {{1.0, 2.0, 7.0}, {8.0, 14.0, 20.0}});

        CHECK(hash_string(A1) == "10111db3db25d77dbebe0d800f0e1f3d");
        CHECK(hash_string(A3) == "689de315b42e3f4cf3f3beeade1bd049");
        CHECK(hash_string(A4) == "04686d96da535aea2efff996e2fc8a35");
        CHECK(hash_string(A5) == "f2a6ab41144279105fffa838e0eeadad");
        CHECK(hash_string(T1) == "924adbe538e98b94f012ee3ed95c784e");
        CHECK(hash_string(T3) == "5290869fabee2937ce49fe39812f2663");
        CHECK(hash_string(T4) == "5022d11a63561f2c90feea810a0812a4");
        CHECK(hash_string(T5) == "334aba0d1868cfa6d0084ee58c521154");
        CHECK(hash_string(T7) == "c6d3311c380bdabf7370f3472f6a04a1");
        CHECK(hash_string(SA1) == "9457f8abf99dec9c4a63e698acb2f5eb");
        CHECK(hash_string(SA2) == "7e6acd7acf4a5096d2576be8021371c5");
        CHECK(hash_string(SA3) == "d60b8ae541c6095aaee8f54ddebb85a7");
    }
}

TEMPLATE_TEST_CASE("TA hashing templated tests for tensors",
                   "[hash][TA][tensor]", int, long, float, double) {
    auto& world      = TA::get_default_world();
    auto hash_string = [](auto a) {
        return bphash::hash_to_string(
          bphash::make_hash(bphash::HashType::Hash128, a));
    };
    TA::TiledRange TR1{{0, 10, 20}, {0, 10, 20}};
    TA::TiledRange TR2{{0, 5, 10}, {0, 5, 10}};
    TA::TiledRange TR3{{0, 15, 30, 40}, {0, 15, 30, 40}};
    TA::TArray<TestType> A0(world, TR1);
    TA::TArray<TestType> A1(world, TR1);
    TA::TArray<TestType> A2(world, TR1);
    TA::TArray<TestType> A3(world, TR2);
    TA::TArray<TestType> A4(world, TR2);
    TA::TArray<TestType> A5(world, TR3);
    A0.fill(0); // If you omit filling, test hangs
    A1.fill(1);
    A2.fill(1);
    A3.fill(1);
    A4.fill(2);
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
    TA::TSpArray<TestType> SA3(world, TR3);
    SA0.fill(0);
    SA1.fill(0);
    SA2.fill(0);
    SA3.fill(1);

    SECTION("TA::DistArray and TA::Tensor relative hash tests") {
        REQUIRE(hash_string(A0) == hash_string(A0));
        REQUIRE(hash_string(A0) != hash_string(A1));
        REQUIRE(hash_string(A1) == hash_string(A2));
        REQUIRE(hash_string(A1) != hash_string(A3));
        REQUIRE(hash_string(A3) != hash_string(A4));
        REQUIRE(hash_string(T1) == hash_string(T1));
        REQUIRE(hash_string(T1) == hash_string(T2));
        REQUIRE(hash_string(T2) != hash_string(T3));
        REQUIRE(hash_string(T2) != hash_string(T4));
        REQUIRE(hash_string(T3) != hash_string(T4));
        REQUIRE(hash_string(T5) == hash_string(T6));
        REQUIRE(hash_string(T6) != hash_string(T7));
        REQUIRE(hash_string(SA0) == hash_string(SA0));
        REQUIRE(hash_string(SA0) == hash_string(SA1));
        REQUIRE(hash_string(SA1) != hash_string(SA2));
        REQUIRE(hash_string(SA2) != hash_string(SA3));
    }
}
