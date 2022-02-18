#include "chemist/detail_/view_base.hpp"
#include <catch2/catch.hpp>

/* Testing Strategy:
 *
 * The ViewBase class is intended to be used with a class containing aliased
 * state such that the class will deep copy the state when copied. It also must
 * have an operator==. std::vector satisfies these conditions so long as we
 * know the address of the buffer.
 *
 * Specific things we need to check:
 * - Value ctor does not break aliasing by copying the instance
 * - Copy ctor/assingment is shallow
 * - Can compare to the wrapped type
 * - returns the pimpl in a const-correct manner
 */
using vector_t = std::vector<int>;

// A facade struct for testing read-/write-able views
template<bool is_const>
struct ViewTester : chemist::detail_::ViewBase<is_const, vector_t> {
    using base_type = chemist::detail_::ViewBase<is_const, vector_t>;
    explicit ViewTester(vector_t v) : base_type(std::move(v)) {}
    decltype(auto) check_pimpl() { return this->pimpl(); }
    decltype(auto) check_pimpl() const { return this->pimpl(); }
};

using view_t       = ViewTester<false>;
using const_view_t = ViewTester<true>;

TEST_CASE("ViewBase : value ctor") {
    vector_t v{1, 2, 3};
    int* pv = v.data();
    view_t view(std::move(v));
    REQUIRE(pv == view.check_pimpl().data());
}

TEST_CASE("const ViewBase : value ctor") {
    vector_t v{1, 2, 3};
    int* pv = v.data();
    const_view_t view(std::move(v));
    REQUIRE(pv == view.check_pimpl().data());
}

TEST_CASE("ViewBase : copy ctor") {
    vector_t v{1, 2, 3};
    int* pv = v.data();
    view_t view(std::move(v));
    view_t view2(view);
    REQUIRE(pv == view2.check_pimpl().data());
}

TEST_CASE("const ViewBase : copy ctor") {
    vector_t v{1, 2, 3};
    int* pv = v.data();
    const_view_t view(std::move(v));
    const_view_t view2(view);
    REQUIRE(pv == view2.check_pimpl().data());
}

TEST_CASE("ViewBase : copy assignment") {
    vector_t v{1, 2, 3};
    int* pv = v.data();
    view_t view(std::move(v));
    view_t view2(vector_t{});
    auto pv2 = &(view2 = view);
    SECTION("Returns this") { REQUIRE(pv2 == &view2); }
    SECTION("State") { REQUIRE(pv == view2.check_pimpl().data()); }
}

TEST_CASE("const ViewBase : copy assignment") {
    vector_t v{1, 2, 3};
    int* pv = v.data();
    const_view_t view(std::move(v));
    const_view_t view2(vector_t{});
    auto pv2 = &(view2 = view);
    SECTION("Returns this") { REQUIRE(pv2 == &view2); }
    REQUIRE(pv == view2.check_pimpl().data());
}

TEST_CASE("ViewBase : move ctor") {
    vector_t v{1, 2, 3};
    int* pv = v.data();
    view_t view(std::move(v));
    view_t view2(std::move(view));
    REQUIRE(pv == view2.check_pimpl().data());
}

TEST_CASE("const ViewBase : move ctor") {
    vector_t v{1, 2, 3};
    int* pv = v.data();
    const_view_t view(std::move(v));
    const_view_t view2(std::move(view));
    REQUIRE(pv == view2.check_pimpl().data());
}

TEST_CASE("ViewBase : move assignment") {
    vector_t v{1, 2, 3};
    int* pv = v.data();
    view_t view(std::move(v));
    view_t view2(vector_t{});
    auto pv2 = &(view2 = std::move(view));
    SECTION("Returns this") { REQUIRE(pv2 == &view2); }
    SECTION("State") { REQUIRE(pv == view2.check_pimpl().data()); }
}

TEST_CASE("const ViewBase : move assignment") {
    vector_t v{1, 2, 3};
    int* pv = v.data();
    const_view_t view(std::move(v));
    const_view_t view2(vector_t{});
    auto pv2 = &(view2 = std::move(view));
    SECTION("Returns this") { REQUIRE(pv2 == &view2); }
    REQUIRE(pv == view2.check_pimpl().data());
}

TEST_CASE("ViewBase : implicit conversion") {
    vector_t v{1, 2, 3};
    int* pv = v.data();
    view_t view(std::move(v));
    const vector_t& v2 = view;
    REQUIRE(pv == v2.data());
}

TEST_CASE("const ViewBase : implicit conversion") {
    vector_t v{1, 2, 3};
    int* pv = v.data();
    const_view_t view(std::move(v));
    const vector_t& v2 = view;
    REQUIRE(pv == v2.data());
}

TEST_CASE("ViewBase : View ?= View") {
    vector_t v{1, 2, 3};
    view_t view(v);

    SECTION("Same instance") {
        REQUIRE(view == view);
        REQUIRE_FALSE(view != view);
    }
    SECTION("Same aliased state") {
        view_t view2(v);
        REQUIRE(view == view2);
        REQUIRE_FALSE(view != view2);
    }
    SECTION("Different aliased state") {
        view_t view2(vector_t{4, 5, 6});
        REQUIRE_FALSE(view == view2);
        REQUIRE(view != view2);
    }
}

TEST_CASE("ViewBase : View ?= Value") {
    vector_t v{1, 2, 3};
    view_t view(v);

    SECTION("Same state") {
        REQUIRE(view == v);
        REQUIRE_FALSE(view != v);
    }
    SECTION("Different state") {
        vector_t v2{4, 5, 6};
        REQUIRE_FALSE(view == v2);
        REQUIRE(view != v2);
    }
}

TEST_CASE("ViewBase : Value ?= View") {
    vector_t v{1, 2, 3};
    view_t view(v);

    SECTION("Same state") {
        REQUIRE(v == view);
        REQUIRE_FALSE(v != view);
    }
    SECTION("Different state") {
        vector_t v2{4, 5, 6};
        REQUIRE_FALSE(v2 == view);
        REQUIRE(v2 != view);
    }
}

TEST_CASE("ViewBase : pimpl()") {
    vector_t v{1, 2, 3};
    int* pv = v.data();
    view_t view(std::move(v));
    SECTION("Is read/write") {
        STATIC_REQUIRE(std::is_same_v<decltype(view.check_pimpl()), vector_t&>);
    }
    SECTION("Value") { REQUIRE(pv == view.check_pimpl().data()); }
}

TEST_CASE("const ViewBase : pimpl()") {
    vector_t v{1, 2, 3};
    int* pv = v.data();
    const_view_t view(std::move(v));
    SECTION("Is read-only") {
        using type = decltype(view.check_pimpl());
        STATIC_REQUIRE(std::is_same_v<type, const vector_t&>);
    }
    SECTION("Value") { REQUIRE(pv == view.check_pimpl().data()); }
}

TEST_CASE("ViewBase : pimpl() const") {
    vector_t v{1, 2, 3};
    int* pv = v.data();
    const view_t view(std::move(v));
    SECTION("Is read-only") {
        using type = decltype(view.check_pimpl());
        STATIC_REQUIRE(std::is_same_v<type, const vector_t&>);
    }
    SECTION("Value") { REQUIRE(pv == view.check_pimpl().data()); }
}

TEST_CASE("const ViewBase : pimpl() const") {
    vector_t v{1, 2, 3};
    int* pv = v.data();
    const const_view_t view(std::move(v));
    SECTION("Is read-only") {
        using type = decltype(view.check_pimpl());
        STATIC_REQUIRE(std::is_same_v<type, const vector_t&>);
    }
    SECTION("Value") { REQUIRE(pv == view.check_pimpl().data()); }
}
