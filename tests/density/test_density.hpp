#pragma once
#include "../test_libchemist.hpp"
#include "libchemist/orbital_space/ao_space.hpp"
#include "libchemist/orbital_space/derived_space.hpp"

namespace testing {

// Spaces of known densities
using libchemist::orbital_space::AOSpaceD;
using libchemist::orbital_space::DerivedSpaceD;

// Makes appropriate non-default space
template<typename SpaceType>
auto non_default_space() {
    typename AOSpaceD::basis_type bs;
    bs.add_center(libchemist::Center<double>(1.0, 2.0, 3.0));

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