#include <catch2/catch.hpp>
#include <libchemist/ta_helpers/ta_hashers.hpp>

TEST_CASE("TA Hashers with BPHASH") {
    auto& world      = TA::get_default_world();
    auto hash_string = [](auto a) {
        return bphash::hash_to_string(
          bphash::make_hash(bphash::HashType::Hash128, a));
    };
    SECTION("TA::Range") {
        TA::Range R1({0, 0}, {10, 10});
        TA::Range R2({0, 0}, {10, 10});
        TA::Range R3({0, 0}, {5, 5});
        TA::Range R4({5, 10, 20}, {10, 20, 30});
        CHECK(hash_string(R1) == "219a3c981a659cbcdb008e7c108c968e");
        CHECK(hash_string(R2) == hash_string(R1));
        CHECK(hash_string(R3) == "b8bf99f63f56f18d2f15c28d46ba0146");
        CHECK(hash_string(R4) == "d2fa798a228fd12977621b8ad8c21e9d");
    }

    SECTION("TA::TiledRange1 and TA::TiledRange hash:") {
        TA::TiledRange1 tr1{0, 10, 20};
        TA::TiledRange1 tr2{0, 10, 20};
        TA::TiledRange1 tr3{0, 10, 20, 30};
        TA::TiledRange TR1{tr1, tr1};
        TA::TiledRange TR2{{0, 10, 20}, {0, 10, 20}};
        TA::TiledRange TR3{tr3, tr3};
        CHECK(hash_string(tr1) == "c08e8f89c587680aff8246fa9b7027fc");
        CHECK(hash_string(tr2) == hash_string(tr1));
        CHECK(hash_string(tr3) == "f1c3d9235ffea3d662ed18177d6a1d5d");
        CHECK(hash_string(TR1) == "ae8a3122b346a9e806ab13093f3936c5");
        CHECK(hash_string(TR2) == hash_string(TR1));
        CHECK(hash_string(TR3) == "9e292b55cf33cbbdff2be1771d4c789c");
    }
    SECTION("TA::DistArray and TA::Tensor hash") {
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

        CHECK(hash_string(A1) == "4c7e1a8fd4958bd1d158ea7da4e87591");
        CHECK(hash_string(A2) == hash_string(A1));
        CHECK(hash_string(A3) == "adb7922204715819764c65da0af86c66");
        CHECK(hash_string(A4) == "f66adaf3e6603402066d1b0467240393");
        CHECK(hash_string(A5) == "06948afc61299fd459e4a3a7e785d2b2");
        CHECK(hash_string(T1) == "54348d3b54cb70f88837b8921c2908b5");
        CHECK(hash_string(T2) == hash_string(T1));
        CHECK(hash_string(T3) == "ca8ab813a000efe06b369b0c557005f0");
        CHECK(hash_string(T4) == "b37f490e42c6a7f86dfaa6899097e0e4");
        CHECK(hash_string(T5) == "65473550b6c6de81c310b7d98115042e");
        CHECK(hash_string(T6) == hash_string(T5));
        CHECK(hash_string(T7) == "99b4590c4461a81e326e1af58bbea1a6");
        CHECK(hash_string(SA1) == "a4a30981c15fcf545a80c324dab0a0df");
        CHECK(hash_string(SA2) == "c80a36a4c8674451f2d8087acddaaf77");
        CHECK(hash_string(SA3) == "0105821c8e74bd371d890d7e48d8ebed");
    }
}
