#pragma once
#include <chemist/orbital_space/orbital_space.hpp>

using namespace chemist;

namespace {

template<typename T>
auto make_space22(TA::World& world) {
    TA::TSpArray<T> c(world, {{0.12, 0.23}, {0.34, 0.45}});
    return orbital_space::DerivedSpace<T>(c, orbital_space::AOSpace<T>{});
}

template<typename T>
auto make_space23(TA::World& world) {
    TA::TSpArray<T> c(world, {{0.12, 0.23, 0.45}, {0.56, 0.67, 0.78}});
    return orbital_space::DerivedSpace<T>(c, orbital_space::AOSpace<T>{});
}

template<typename T>
auto make_space32(TA::World& world) {
    TA::TSpArray<T> c(world, {{0.12, 0.23}, {0.45, 0.56}, {0.67, 0.78}});
    return orbital_space::DerivedSpace<T>(c, orbital_space::AOSpace<T>{});
}

template<typename T>
auto make_space24(TA::World& world) {
    TA::TSpArray<T> c(world,
                      {{0.12, 0.23, 0.45, 0.56}, {0.67, 0.78, 0.89, 0.90}});
    return orbital_space::DerivedSpace<T>(c, orbital_space::AOSpace<T>{});
}

template<typename T>
auto make_space43(TA::World& world) {
    TA::TSpArray<T> c(world, {{0.12, 0.23, 0.45},
                              {0.56, 0.67, 0.78},
                              {0.89, 0.90, 0.01},
                              {0.12, 0.23, 0.34}});
    return orbital_space::DerivedSpace<T>(c, orbital_space::AOSpace<T>{});
}

template<typename T>
auto make_space25(TA::World& world) {
    TA::TSpArray<T> c(
            world, {{0.12, 0.23, 0.45, 0.56, 0.67}, {0.67, 0.78, 0.89, 0.90, 0.01}});
    return orbital_space::DerivedSpace<T>(c, orbital_space::AOSpace<T>{});
}

} // namespace
