/*
 * Copyright 2022 NWChemEx-Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

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
