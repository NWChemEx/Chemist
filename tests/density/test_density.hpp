#pragma once
#include "../test_chemist.hpp"
#include "chemist/orbital_space/ao_space.hpp"
#include "chemist/orbital_space/derived_space.hpp"

namespace testing {

// Spaces of known densities
using chemist::orbital_space::AOSpaceD;
using chemist::orbital_space::DerivedSpaceD;

// Makes appropriate non-default space
template<typename SpaceType>
auto non_default_space() {
    typename AOSpaceD::basis_type bs;
    bs.add_center(chemist::AtomicBasisSet<double>("", 0, 1.0, 2.0, 3.0));

    if constexpr(std::is_same_v<SpaceType, AOSpaceD>) {
        return SpaceType(bs);
    } else if constexpr(std::is_same_v<SpaceType, DerivedSpaceD>) {
        auto a_tensor = testing::generate_tensor(2);
        return SpaceType(a_tensor, AOSpaceD(bs));
    } else {
        static_assert(std::is_same_v<SpaceType, AOSpaceD>,
                      "SpaceType not coded");
    }
}

} // namespace testing