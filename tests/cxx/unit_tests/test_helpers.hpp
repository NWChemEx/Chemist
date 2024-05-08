/*
 * Copyright 2024 NWChemEx-Project
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
#include <catch2/catch.hpp>
#include <type_traits>

namespace test_chemist {

template<typename T>
void test_copy_ctor(T&& object2test) {
    using clean_type = std::decay_t<T>; // Was given a reference

    clean_type object_copy(object2test);
    REQUIRE(object_copy == object2test); // Check value
}

template<typename T>
void test_copy_assignment(T&& object2test) {
    using clean_type = std::decay_t<T>; // Was given a reference

    clean_type object_copy;
    auto pobject_copy = &(object_copy = object2test);

    REQUIRE(pobject_copy == &object_copy); // Check returns *this
    REQUIRE(object_copy == object2test);   // Check value
}

template<typename T>
void test_move_ctor(T&& object2test) {
    using clean_type = std::decay_t<T>; // Was given a reference

    clean_type object_copy(object2test);
    clean_type object_move(std::move(object2test));
    REQUIRE(object_copy == object_move); // Check value
}

template<typename T>
void test_move_assignment(T&& object2test) {
    using clean_type = std::decay_t<T>; // Was given a reference

    clean_type object_copy(object2test);
    clean_type object_move;
    auto pobject_move = &(object_move = object2test);

    REQUIRE(pobject_move == &object_move); // Check returns *this
    REQUIRE(object_copy == object_move);   // Check value
}

} // namespace test_chemist
