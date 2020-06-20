#include <catch2/catch.hpp>
#include <libchemist/ta_helpers/ta_hashers.hpp>

TEST_CASE("TA Hashers with BPHASH")
{
    auto &world = TA::get_default_world();
    auto hash_string = [](auto a) { return bphash::hash_to_string(bphash::make_hash(bphash::HashType::Hash128, a)); };
    SECTION("TA::Range")
    {
        TA::Range R1({0, 0}, {10, 10});
        TA::Range R2({0, 0}, {10, 10});
        TA::Range R3({0, 0}, {5, 5});
        TA::Range R4({5, 10, 20}, {10, 20, 30});
        REQUIRE(hash_string(R1) == "219a3c981a659cbcdb008e7c108c968e");
        REQUIRE(hash_string(R2) == hash_string(R1));
        REQUIRE(hash_string(R3) == "b8bf99f63f56f18d2f15c28d46ba0146");
        REQUIRE(hash_string(R4) == "d2fa798a228fd12977621b8ad8c21e9d");
    }

    SECTION("TA::TiledRange1 and TA::TiledRange hash:")
    {
        TA::TiledRange1 tr1{0, 10, 20};
        TA::TiledRange1 tr2{0, 10, 20};
        TA::TiledRange1 tr3{0, 10, 20, 30};
        TA::TiledRange TR1{tr1, tr1};
        TA::TiledRange TR2{{0, 10, 20},
                           {0, 10, 20}};
        TA::TiledRange TR3{tr3, tr3};
        REQUIRE(hash_string(tr1) == "c08e8f89c587680aff8246fa9b7027fc");
        REQUIRE(hash_string(tr2) == hash_string(tr1));
        REQUIRE(hash_string(tr3) == "f1c3d9235ffea3d662ed18177d6a1d5d");
        REQUIRE(hash_string(TR1) == "ae8a3122b346a9e806ab13093f3936c5");
        REQUIRE(hash_string(TR2) == hash_string(TR1));
        REQUIRE(hash_string(TR3) == "9e292b55cf33cbbdff2be1771d4c789c");
    }
    SECTION("TA::DistArray and TA::Tensor hash")
    {
        TA::TiledRange TR1{{0, 10, 20},
                           {0, 10, 20}};
        TA::TiledRange TR2{{0, 5, 10},
                           {0, 5, 10}};
        TA::TiledRange TR3{{0, 15, 30, 40},
                           {0, 15, 30, 40}};
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
        TA::TensorD T6 = A1.find({1, 0}).get();
        TA::TensorD T7 = A3.find({0, 0}).get();
        TA::TSpArrayD SA1(world, {{1.0, 2.0}, {3.0, 4.0}});
        TA::TSpArrayD SA2(world, {{10.0, 20.0}, {30.0, 40.0}});
        TA::TSpArrayD SA3(world, {{1.0, 2.0, 7.0}, {8.0, 14.0, 20.0}});

        REQUIRE(hash_string(A1) == "0a7431c088aaeafa3ddccf72203aadea");
        REQUIRE(hash_string(A2) == hash_string(A1));
        REQUIRE(hash_string(A3) == "0c4951ce31346b25fb166f641cbb0d92");
        REQUIRE(hash_string(A4) == "065cf028d8047c6fe0ac18cf8011b6e9");
        REQUIRE(hash_string(A5) == "c0bfdd73b6610cbde19ad5dda1491871");
        REQUIRE(hash_string(T1) == "6caaa572b30c21718b06ac70a18d351f");
        REQUIRE(hash_string(T2) == hash_string(T1));
        REQUIRE(hash_string(T3) == "8a414b24d57a4d30ec53267eada373bd");
        REQUIRE(hash_string(T4) == "83a3c67988c3a6c8b125d1be70d0ed4f");
        REQUIRE(hash_string(T5) == "a798d2a83a0e4b215b02319b55f2a009");
        REQUIRE(hash_string(T6) == hash_string(T5));
        REQUIRE(hash_string(T7) == "f9f4bf7b7f21d08813db8dc9a6111f83");
        REQUIRE(hash_string(SA1) == "c949ad2ebbc17ead3b36d95eca7e3f30");
        REQUIRE(hash_string(SA2) == "2a0c74c07a6d08dc9da960a11c607bcb");
        REQUIRE(hash_string(SA3) == "9194b30b5f1ca8bf87525da8c204b558");
    }
}
