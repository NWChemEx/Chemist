#include <catch2/catch.hpp>
#include <chemist/canonical_mos.hpp>
#include <chemist/chemist.hpp>
#include <chemist/orthogonal_space.hpp>
#include <chemist/ta_helpers/ta_helpers.hpp>
#include <sde/hasher.hpp>

using sde::hash_objects;
using vector_t   = std::initializer_list<double>;
using matrix_t   = std::initializer_list<vector_t>;
using tensor     = chemist::type::tensor<double>;
using orbspace   = chemist::OrbitalSpace<double, tensor>;
using aospace    = chemist::AOSpace<double, tensor>;
using orthspace  = chemist::OrthogonalSpace<double, tensor>;
using canonspace = chemist::CanonicalMO<double, tensor>;

static inline auto make_bs() {
    using chemist::Atom;
    using c_t = typename Atom::coord_type;

    Atom O{8ul, c_t{0.000000000000000, -0.143222342980786, 0.000000000000000}};
    Atom H1{1ul, c_t{1.638033502034240, 1.136556880358410, 0.000000000000000}};
    Atom H2{1ul, c_t{-1.638033502034240, 1.136556880358410, 0.000000000000000}};
    auto water = chemist::Molecule(O, H1, H2);
    return chemist::apply_basis("sto-3g", water);
}

void require_not_initialized(orbspace& space) {
    REQUIRE(space.basis_set().empty());
    REQUIRE(!space.S().is_initialized());
    REQUIRE(!space.C().is_initialized());
    REQUIRE(!space.Cdagger().is_initialized());
    REQUIRE(!space.density().is_initialized());
}

void require_not_initialized_canon(canonspace& space) {
    require_not_initialized(space);
    REQUIRE(!space.mo_energies().is_initialized());
}

void require_match_inputs(orbspace& space, chemist::AOBasisSet<double> bs,
                          tensor A, tensor B, tensor C, tensor D) {
    REQUIRE(space.basis_set() == bs);
    REQUIRE(chemist::ta_helpers::allclose(space.S(), A));
    REQUIRE(chemist::ta_helpers::allclose(space.C(), B));
    REQUIRE(chemist::ta_helpers::allclose(space.Cdagger(), C));
    REQUIRE(chemist::ta_helpers::allclose(space.density(), D));
}

void require_match_inputs_canon(canonspace space,
                                chemist::AOBasisSet<double> bs, tensor A,
                                tensor B, tensor C, tensor D, tensor E) {
    require_match_inputs(space, bs, A, B, C, D);
    REQUIRE(chemist::ta_helpers::allclose(space.mo_energies(), E));
}

static matrix_t S_il{
  {
    1.0000000000000004,
    0.2367039365108476,
    0.0000000000000000,
    0.0000000000000000,
    0.0000000000000000,
    0.0384055905135491,
    0.0384055905135491,
  },
  {
    0.2367039365108476,
    1.0000000000000002,
    0.0000000000000000,
    0.0000000000000000,
    0.0000000000000000,
    0.3861387813310929,
    0.3861387813310929,
  },
  {
    0.0000000000000000,
    0.0000000000000000,
    1.0000000000000007,
    0.0000000000000000,
    0.0000000000000000,
    0.2097276494226498,
    0.2097276494226498,
  },
  {
    0.0000000000000000,
    0.0000000000000000,
    0.0000000000000000,
    1.0000000000000002,
    0.0000000000000000,
    0.0000000000000000,
    0.0000000000000000,
  },
  {
    0.0000000000000000,
    0.0000000000000000,
    0.0000000000000000,
    0.0000000000000000,
    1.0000000000000007,
    0.2684376412681763,
    -0.2684376412681763,
  },
  {
    0.0384055905135491,
    0.3861387813310928,
    0.2097276494226498,
    0.0000000000000000,
    0.2684376412681763,
    1.0000000000000002,
    0.1817608668218930,
  },
  {
    0.0384055905135491,
    0.3861387813310928,
    0.2097276494226498,
    0.0000000000000000,
    -0.2684376412681763,
    0.1817608668218930,
    1.0000000000000002,
  },
};

static matrix_t C_il{
  {-0.9944345909859650, -0.2391588539711208, -0.0000000000000001,
   -0.0936833932744580, 0.0000000000000004, 0.1116397621621844,
   0.0000000000000001},
  {-0.0240970370894384, 0.8857356579771968, 0.0000000000000009,
   0.4795866103044956, -0.0000000000000019, -0.6695782145120205,
   -0.0000000000000003},
  {-0.0031615543085716, 0.0858963283629680, 0.0000000000000003,
   -0.7474302531038645, 0.0000000000000027, -0.7384902083769633,
   -0.0000000000000005},
  {-0.0000000000000000, 0.0000000000000000, 0.0000000000000002,
   -0.0000000000000041, -0.9999999999999998, 0.0000000000000001,
   0.0000000000000001},
  {-0.0000000000000000, -0.0000000000000001, -0.6072855567155301,
   0.0000000000000003, -0.0000000000000001, -0.0000000000000012,
   0.9192333316155601},
  {0.0045937403566917, 0.1440394600395703, -0.4529976671667624,
   -0.3294717071548038, 0.0000000000000013, 0.7098489962603716,
   -0.7324609630613095},
  {0.0045937403566917, 0.1440394600395708, 0.4529976671667600,
   -0.3294717071548048, 0.0000000000000014, 0.7098489962603722,
   0.7324609630613099},
};

static matrix_t D_il{
  {
    1.0548736913576202,
    -0.2327978986834940,
    0.0526228938608669,
    0.0000000000000000,
    0.0000000000000001,
    -0.0081504589881945,
    -0.0081504589881947,
  },
  {
    -0.2327978986834940,
    1.0151116397921436,
    -0.2822999165132808,
    -0.0000000000000000,
    -0.0000000000000005,
    -0.0305400288445337,
    -0.0305400288445329,
  },
  {
    0.0526228938608669,
    -0.2822999165132808,
    0.5660401579067917,
    0.0000000000000004,
    -0.0000000000000004,
    0.2586150588664435,
    0.2586150588664446,
  },
  {
    0.0000000000000000,
    -0.0000000000000000,
    0.0000000000000004,
    0.9999999999999996,
    0.0000000000000000,
    -0.0000000000000000,
    0.0000000000000000,
  },
  {
    0.0000000000000001,
    -0.0000000000000005,
    -0.0000000000000004,
    0.0000000000000000,
    0.3687957473952913,
    0.2750989404962036,
    -0.2750989404962024,
  },
  {
    -0.0081504589881945,
    -0.0305400288445337,
    0.2586150588664435,
    -0.0000000000000000,
    0.2750989404962036,
    0.3345269607729853,
    -0.0758868121440709,
  },
  {
    -0.0081504589881947,
    -0.0305400288445329,
    0.2586150588664446,
    0.0000000000000000,
    -0.2750989404962024,
    -0.0758868121440709,
    0.3345269607729840,
  },
};

TEST_CASE("Orbital space") {
    auto& world = TA::get_default_world();
    auto bs     = make_bs();

    auto AO_range  = TA::TiledRange1(0, 5, 6, 7); // atomic tiling
    auto OCC_range = TA::TiledRange1(0, 5);       // occupied only

    auto AO_trange = TA::TiledRange({AO_range, AO_range});
    auto MO_trange = TA::TiledRange({AO_range, OCC_range});

    tensor S(world, AO_trange, S_il);
    tensor C(world, MO_trange, C_il);
    tensor D(world, AO_trange, D_il);
    tensor Cdagger;
    Cdagger("i, mu") = C("mu, i");

    auto I_ao = TA::diagonal_array<tensor, double>(
      world, TA::TiledRange({AO_range, AO_range}));
    auto I_mo = TA::diagonal_array<tensor, double>(
      world, TA::TiledRange({OCC_range, OCC_range}));

    // Test OrbitalSpace
    orbspace Space0;
    require_not_initialized(Space0);

    orbspace Space1(bs, S, C, Cdagger, D);
    require_match_inputs(Space1, bs, S, C, Cdagger, D);

    orbspace Space2(bs, S, C, Cdagger);
    require_match_inputs(Space2, bs, S, C, Cdagger, D);
    REQUIRE(Space1 == Space2);
    REQUIRE(hash_objects(Space1) == hash_objects(Space2));

    orbspace Space3(bs, S, C);
    require_match_inputs(Space3, bs, S, C, Cdagger, D);

    orbspace Space4(Space1);
    REQUIRE(Space1 == Space4);
    REQUIRE(hash_objects(Space1) == hash_objects(Space4));

    orbspace Space5;
    auto pSpace5 = &(Space5 = Space1);
    REQUIRE(pSpace5 == &Space5);
    REQUIRE(&Space1.C() != &Space5.C());

    orbspace Space6(std::move(Space2));
    REQUIRE(Space1 == Space6);
    REQUIRE(hash_objects(Space1) == hash_objects(Space6));

    orbspace Space7;
    auto pSpace7 = &(Space7 = std::move(Space3));
    REQUIRE(pSpace7 == &Space7);
    REQUIRE(Space1 == Space7);
    REQUIRE(hash_objects(Space1) == hash_objects(Space7));

    orbspace Space8(bs, S, I_ao);
    require_match_inputs(Space8, bs, S, I_ao, I_ao, I_ao);
    REQUIRE(Space1 != Space8);
    REQUIRE(hash_objects(Space1) != hash_objects(Space8));

    auto E = Space1.transform_from_ao(S, {0, 1});
    REQUIRE(chemist::ta_helpers::allclose(E, I_mo));

    auto F = Space1.transform_to_ao(I_mo, {0, 1});
    REQUIRE(chemist::ta_helpers::allclose(F, D));

    auto G = Space1.transform_to_ao(C, {1});
    REQUIRE(chemist::ta_helpers::allclose(G, D));

    // Test AOSpace
    aospace AO1;
    require_not_initialized(AO1);

    auto H = AO1.transform_from_ao(S, {0, 1});
    REQUIRE(chemist::ta_helpers::allclose(H, S));

    auto I = AO1.transform_to_ao(S, {0, 1});
    REQUIRE(chemist::ta_helpers::allclose(I, S));

    aospace AO2(bs, S);
    require_match_inputs(AO2, bs, S, I_ao, I_ao, S);

    // Test OrthogonalSpace
    orthspace orth0;
    orthspace orth1(bs, S, C, Cdagger, D);
    orthspace orth2(bs, S, C, Cdagger);
    orthspace orth3(bs, S, C);
    require_not_initialized(orth0);
    require_match_inputs(orth1, bs, S, C, Cdagger, D);
    require_match_inputs(orth3, bs, S, C, Cdagger, D);
    require_match_inputs(orth3, bs, S, C, Cdagger, D);
    REQUIRE(orth1 == Space1);

    // Test CanonicalMO
    // Using whatever is on hand for the mo energies, since the values don't
    // matter
    canonspace canmo0;
    canonspace canmo1(bs, S, I_ao, C, Cdagger, D);
    canonspace canmo2(bs, S, I_ao, C, Cdagger);
    canonspace canmo3(bs, S, I_ao, C);
    canonspace canmo4(bs, S, I_mo, C);
    require_not_initialized_canon(canmo0);
    require_match_inputs_canon(canmo1, bs, S, C, Cdagger, D, I_ao);
    require_match_inputs_canon(canmo2, bs, S, C, Cdagger, D, I_ao);
    require_match_inputs_canon(canmo3, bs, S, C, Cdagger, D, I_ao);
    REQUIRE(Space1 == canmo1);
    REQUIRE(orth1 == canmo1);
    REQUIRE(canmo1 == canmo2);
    REQUIRE(canmo1 != canmo4);
}
