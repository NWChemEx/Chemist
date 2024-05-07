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
