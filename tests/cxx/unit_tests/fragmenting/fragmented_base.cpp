#include <catch2/catch.hpp>
#include <chemist/fragmenting/fragmented_base.hpp>
#include <chemist/fragmenting/fragmented_nuclei.hpp>

using namespace chemist;
using namespace chemist::fragmenting;
using types2test =
  std::tuple<FragmentedNuclei<Nuclei>, FragmentedNuclei<const Nuclei>>;

TEMPLATE_LIST_TEST_CASE("FragmentedBase<T>", "", types2test) {
    // Type we're testing
    using class_type = FragmentedBase<TestType>;
    // cv-qualified supersystem type
    using supersystem_type = typename class_type::traits_type::supersystem_type;
    // supersystem type w/o cv qualifiers
    using no_cv_t = std::remove_cv_t<supersystem_type>;
    // Type of objects in the supersystem
    using element_type = typename no_cv_t::value_type;

    supersystem_type empty_supersystem;
    supersystem_type value_supersystem{element_type{}};

    class_type defaulted;
    class_type empty(empty_supersystem);
    class_type value(value_supersystem);

    SECTION("Ctor") {
        SECTION("default") { REQUIRE(defaulted.is_null()); }

        SECTION("value") {
            REQUIRE_FALSE(empty.is_null());
            REQUIRE(empty.supersystem() == empty_supersystem);
            REQUIRE(value.supersystem() == value_supersystem);
        }

        SECTION("copy") {
            class_type defaulted_copy(defaulted);
            REQUIRE(defaulted_copy.class_type::operator==(defaulted));

            class_type empty_copy(empty);
            REQUIRE(empty_copy.class_type::operator==(empty));

            class_type value_copy(value);
            REQUIRE(value_copy.class_type::operator==(value));
        }

        SECTION("move") {
            class_type defaulted_copy(defaulted);
            class_type defaulted_move(std::move(defaulted));
            REQUIRE(defaulted_copy.class_type::operator==(defaulted_move));

            class_type empty_copy(empty);
            class_type empty_move(std::move(empty));
            REQUIRE(empty_copy.class_type::operator==(empty_move));

            class_type value_copy(value);
            class_type value_move(std::move(value));
            REQUIRE(value_copy.class_type::operator==(value_move));
        }

        SECTION("copy assignment") {
            class_type defaulted_copy;
            auto pdefaulted_copy = &(defaulted_copy = defaulted);
            REQUIRE(pdefaulted_copy == &defaulted_copy);
            REQUIRE(defaulted_copy.class_type::operator==(defaulted));

            class_type empty_copy;
            auto pempty_copy = &(empty_copy = empty);
            REQUIRE(pempty_copy == &empty_copy);
            REQUIRE(empty_copy.class_type::operator==(empty));

            class_type value_copy;
            auto pvalue_copy = &(value_copy = value);
            REQUIRE(pvalue_copy == &value_copy);
            REQUIRE(value_copy.class_type::operator==(value));
        }

        SECTION("move assignment") {
            class_type defaulted_copy(defaulted);
            class_type defaulted_move;
            auto pdefaulted_move = &(defaulted_move = std::move(defaulted));
            REQUIRE(pdefaulted_move == &defaulted_move);
            REQUIRE(defaulted_copy.class_type::operator==(defaulted_move));

            class_type empty_copy(empty);
            class_type empty_move;
            auto pempty_move = &(empty_move = std::move(empty));
            REQUIRE(pempty_move == &empty_move);
            REQUIRE(empty_copy.class_type::operator==(empty_move));

            class_type value_copy(value);
            class_type value_move;
            auto pvalue_move = &(value_move = std::move(value));
            REQUIRE(pvalue_move == &value_move);
            REQUIRE(value_copy.class_type::operator==(value_move));
        }
    }

    SECTION("supersystem") {
        REQUIRE_THROWS_AS(defaulted.supersystem(), std::bad_optional_access);

        REQUIRE(empty.supersystem() == empty_supersystem);
        REQUIRE(value.supersystem() == value_supersystem);

        // For non-const supersystems can we modify them?
        if constexpr(std::is_same_v<supersystem_type, no_cv_t>) {
            value.supersystem()[0].x() = 1.0;
            REQUIRE(value.supersystem()[0].x() == 1.0);

            // Shouldn't be aliasing value_supersystem
            REQUIRE(value_supersystem[0].x() == 0.0);
        }
    }

    SECTION("supersystem() const") {
        using except_t = std::bad_optional_access;
        REQUIRE_THROWS_AS(std::as_const(defaulted).supersystem(), except_t);

        REQUIRE(std::as_const(empty).supersystem() == empty_supersystem);
        REQUIRE(std::as_const(value).supersystem() == value_supersystem);
    }

    SECTION("is_null") {
        REQUIRE(defaulted.is_null());
        REQUIRE_FALSE(empty.is_null());
        REQUIRE_FALSE(value.is_null());
    }

    SECTION("operator==") {
        // N.B. we want to avoid calling IndexableContainer::operator==

        // Null vs. null
        REQUIRE(defaulted.class_type::operator==(class_type{}));

        // Null vs. empty
        REQUIRE_FALSE(defaulted.class_type::operator==(empty));

        // Null vs. value
        REQUIRE_FALSE(defaulted.class_type::operator==(value));

        // Empty vs. empty
        REQUIRE(empty.class_type::operator==(class_type(empty_supersystem)));

        // Empty vs. value (essentially a check on different supersystems)
        REQUIRE_FALSE(empty.class_type::operator==(value));
    }

    SECTION("operator!=") {
        // Just negates operator==, so spot check a true and a false case
        REQUIRE(defaulted.class_type::operator!=(empty));
        REQUIRE_FALSE(defaulted.class_type::operator!=(class_type{}));
    }
}
