#include <catch2/catch.hpp>
#include <chemist/basis_set2/contracted_gaussian/detail_/contracted_gaussian_view_pimpl.hpp>
#include <chemist/basis_set2/primitive/primitive.hpp>

using namespace chemist::basis_set;
using namespace chemist::basis_set::detail_;

template<typename LHSType, typename RHSType>
void compare_addresses(LHSType&& lhs, RHSType&& rhs) {
    REQUIRE(&lhs.coefficient(0) == &rhs[0].coefficient());
    REQUIRE(&lhs.exponent(0) == &rhs[0].exponent());
    REQUIRE(&lhs.center().x() == &rhs.center().x());
    REQUIRE(&lhs.center().y() == &rhs.center().y());
    REQUIRE(&lhs.center().z() == &rhs.center().z());
}

TEMPLATE_TEST_CASE("ContractedGaussianViewPIMPL", "", PrimitiveF, PrimitiveD) {
    using cg_type    = ContractedGaussian<TestType>;
    using view_type  = ContractedGaussianViewPIMPL<cg_type>;
    using const_view = ContractedGaussianViewPIMPL<const cg_type>;

    using cg_traits = ContractedGaussianTraits<cg_type>;
    using prim_type = typename cg_traits::primitive_type;

    using center_type = typename cg_traits::center_type;
    using c_vector    = std::vector<typename cg_traits::coefficient_type>;
    using e_vector    = std::vector<typename cg_traits::exponent_type>;

    center_type r0, r1(1.0, 2.0, 3.0);
    c_vector cs{4.0, 5.0, 6.0};
    e_vector es{7.0, 8.0, 9.0};

    cg_type cg0;
    cg_type cg1(cs.begin(), cs.end(), es.begin(), es.end(), r1);

    view_type cg0_view;
    view_type cg1_view(cg1.size(), cg1[0].coefficient(), cg1[0].exponent(),
                       cg1.center());
    const_view const_cg0_view;
    const_view const_cg1_view(cg1.size(), cg1[0].coefficient(),
                              cg1[0].exponent(), cg1.center());

    SECTION("Ctors and assignment") {
        SECTION("Default") {
            REQUIRE(cg0_view.size() == 0);
            REQUIRE(const_cg0_view.size() == 0);
        }

        SECTION("Value") {
            REQUIRE(cg1_view.size() == 3);
            REQUIRE(cg1_view.coefficient(0) == 4.0);
            REQUIRE(cg1_view.coefficient(1) == 5.0);
            REQUIRE(cg1_view.coefficient(2) == 6.0);
            REQUIRE(cg1_view.exponent(0) == 7.0);
            REQUIRE(cg1_view.exponent(1) == 8.0);
            REQUIRE(cg1_view.exponent(2) == 9.0);
            compare_addresses(cg1_view, cg1);

            REQUIRE(const_cg1_view.size() == 3);
            REQUIRE(const_cg1_view.coefficient(0) == 4.0);
            REQUIRE(const_cg1_view.coefficient(1) == 5.0);
            REQUIRE(const_cg1_view.coefficient(2) == 6.0);
            REQUIRE(const_cg1_view.exponent(0) == 7.0);
            REQUIRE(const_cg1_view.exponent(1) == 8.0);
            REQUIRE(const_cg1_view.exponent(2) == 9.0);
            compare_addresses(const_cg1_view, cg1);
        }

        SECTION("copy") {
            view_type copy_cg0(cg0_view);
            REQUIRE(copy_cg0.size() == 0);

            const_view const_copy_cg0(const_cg0_view);
            REQUIRE(const_copy_cg0.size() == 0);

            view_type copy_cg1_view(cg1_view);
            REQUIRE(copy_cg1_view.size() == 3);
            REQUIRE(copy_cg1_view.coefficient(0) == 4.0);
            REQUIRE(copy_cg1_view.coefficient(1) == 5.0);
            REQUIRE(copy_cg1_view.coefficient(2) == 6.0);
            REQUIRE(copy_cg1_view.exponent(0) == 7.0);
            REQUIRE(copy_cg1_view.exponent(1) == 8.0);
            REQUIRE(copy_cg1_view.exponent(2) == 9.0);
            compare_addresses(copy_cg1_view, cg1);

            const_view copy_const_cg1_view(const_cg1_view);
            REQUIRE(copy_const_cg1_view.size() == 3);
            REQUIRE(copy_const_cg1_view.coefficient(0) == 4.0);
            REQUIRE(copy_const_cg1_view.coefficient(1) == 5.0);
            REQUIRE(copy_const_cg1_view.coefficient(2) == 6.0);
            REQUIRE(copy_const_cg1_view.exponent(0) == 7.0);
            REQUIRE(copy_const_cg1_view.exponent(1) == 8.0);
            REQUIRE(copy_const_cg1_view.exponent(2) == 9.0);
        }

        SECTION("copy assignment") {
            view_type copy_cg0;
            auto pcopy_cg0 = &(copy_cg0 = cg0_view);
            REQUIRE(copy_cg0.size() == 0);
            REQUIRE(pcopy_cg0 == &copy_cg0);

            const_view const_copy_cg0;
            auto pconst_copy_cg0 = &(const_copy_cg0 = const_cg0_view);
            REQUIRE(const_copy_cg0.size() == 0);
            REQUIRE(pconst_copy_cg0 == &const_copy_cg0);

            view_type copy_cg1_view;
            auto pcopy_cg1_view = &(copy_cg1_view = cg1_view);
            REQUIRE(copy_cg1_view.size() == 3);
            REQUIRE(copy_cg1_view.coefficient(0) == 4.0);
            REQUIRE(copy_cg1_view.coefficient(1) == 5.0);
            REQUIRE(copy_cg1_view.coefficient(2) == 6.0);
            REQUIRE(copy_cg1_view.exponent(0) == 7.0);
            REQUIRE(copy_cg1_view.exponent(1) == 8.0);
            REQUIRE(copy_cg1_view.exponent(2) == 9.0);
            compare_addresses(copy_cg1_view, cg1);
            REQUIRE(pcopy_cg1_view == &copy_cg1_view);

            const_view copy_const_cg1_view;
            auto pcopy_const_cg1_view = &(copy_const_cg1_view = const_cg1_view);
            REQUIRE(copy_const_cg1_view.size() == 3);
            REQUIRE(copy_const_cg1_view.coefficient(0) == 4.0);
            REQUIRE(copy_const_cg1_view.coefficient(1) == 5.0);
            REQUIRE(copy_const_cg1_view.coefficient(2) == 6.0);
            REQUIRE(copy_const_cg1_view.exponent(0) == 7.0);
            REQUIRE(copy_const_cg1_view.exponent(1) == 8.0);
            REQUIRE(copy_const_cg1_view.exponent(2) == 9.0);
            REQUIRE(pcopy_const_cg1_view == &copy_const_cg1_view);
        }

        SECTION("move") {
            view_type move_cg0(std::move(cg0_view));
            REQUIRE(move_cg0.size() == 0);

            const_view const_move_cg0(std::move(const_cg0_view));
            REQUIRE(const_move_cg0.size() == 0);

            view_type move_cg1_view(std::move(cg1_view));
            REQUIRE(move_cg1_view.size() == 3);
            REQUIRE(move_cg1_view.coefficient(0) == 4.0);
            REQUIRE(move_cg1_view.coefficient(1) == 5.0);
            REQUIRE(move_cg1_view.coefficient(2) == 6.0);
            REQUIRE(move_cg1_view.exponent(0) == 7.0);
            REQUIRE(move_cg1_view.exponent(1) == 8.0);
            REQUIRE(move_cg1_view.exponent(2) == 9.0);
            compare_addresses(move_cg1_view, cg1);

            const_view move_const_cg1_view(std::move(const_cg1_view));
            REQUIRE(move_const_cg1_view.size() == 3);
            REQUIRE(move_const_cg1_view.coefficient(0) == 4.0);
            REQUIRE(move_const_cg1_view.coefficient(1) == 5.0);
            REQUIRE(move_const_cg1_view.coefficient(2) == 6.0);
            REQUIRE(move_const_cg1_view.exponent(0) == 7.0);
            REQUIRE(move_const_cg1_view.exponent(1) == 8.0);
            REQUIRE(move_const_cg1_view.exponent(2) == 9.0);
        }

        SECTION("move assignment") {
            view_type move_cg0;
            auto pmove_cg0 = &(move_cg0 = std::move(cg0_view));
            REQUIRE(move_cg0.size() == 0);
            REQUIRE(pmove_cg0 == &move_cg0);

            const_view const_move_cg0;
            auto pconst_move_cg0 =
              &(const_move_cg0 = std::move(const_cg0_view));
            REQUIRE(const_move_cg0.size() == 0);
            REQUIRE(pconst_move_cg0 == &const_move_cg0);

            view_type move_cg1_view;
            auto pmove_cg1_view = &(move_cg1_view = std::move(cg1_view));
            REQUIRE(move_cg1_view.size() == 3);
            REQUIRE(move_cg1_view.coefficient(0) == 4.0);
            REQUIRE(move_cg1_view.coefficient(1) == 5.0);
            REQUIRE(move_cg1_view.coefficient(2) == 6.0);
            REQUIRE(move_cg1_view.exponent(0) == 7.0);
            REQUIRE(move_cg1_view.exponent(1) == 8.0);
            REQUIRE(move_cg1_view.exponent(2) == 9.0);
            compare_addresses(move_cg1_view, cg1);
            REQUIRE(pmove_cg1_view == &move_cg1_view);

            const_view move_const_cg1_view;
            auto pmove_const_cg1_view =
              &(move_const_cg1_view = std::move(const_cg1_view));
            REQUIRE(move_const_cg1_view.size() == 3);
            REQUIRE(move_const_cg1_view.coefficient(0) == 4.0);
            REQUIRE(move_const_cg1_view.coefficient(1) == 5.0);
            REQUIRE(move_const_cg1_view.coefficient(2) == 6.0);
            REQUIRE(move_const_cg1_view.exponent(0) == 7.0);
            REQUIRE(move_const_cg1_view.exponent(1) == 8.0);
            REQUIRE(move_const_cg1_view.exponent(2) == 9.0);
            REQUIRE(pmove_const_cg1_view == &move_const_cg1_view);
        }
    }

    SECTION("Getters and setters") {
        // N.B. The PIMPL does not do bounds checking so tests will likely
        // segfault if they go out of bounds

        SECTION("size") {
            REQUIRE(cg0_view.size() == 0);
            REQUIRE(const_cg0_view.size() == 0);
            REQUIRE(cg1_view.size() == 3);
            REQUIRE(const_cg1_view.size() == 3);
        }

        SECTION("coefficient()") {
            REQUIRE(cg1_view.coefficient(0) == 4.0);
            REQUIRE(cg1_view.coefficient(1) == 5.0);
            REQUIRE(cg1_view.coefficient(2) == 6.0);

            REQUIRE(const_cg1_view.coefficient(0) == 4.0);
            REQUIRE(const_cg1_view.coefficient(1) == 5.0);
            REQUIRE(const_cg1_view.coefficient(2) == 6.0);
        }

        SECTION("coefficient() const") {
            REQUIRE(std::as_const(cg1_view).coefficient(0) == 4.0);
            REQUIRE(std::as_const(cg1_view).coefficient(1) == 5.0);
            REQUIRE(std::as_const(cg1_view).coefficient(2) == 6.0);

            REQUIRE(std::as_const(const_cg1_view).coefficient(0) == 4.0);
            REQUIRE(std::as_const(const_cg1_view).coefficient(1) == 5.0);
            REQUIRE(std::as_const(const_cg1_view).coefficient(2) == 6.0);
        }

        SECTION("exponent()") {
            REQUIRE(cg1_view.exponent(0) == 7.0);
            REQUIRE(cg1_view.exponent(1) == 8.0);
            REQUIRE(cg1_view.exponent(2) == 9.0);

            REQUIRE(const_cg1_view.exponent(0) == 7.0);
            REQUIRE(const_cg1_view.exponent(1) == 8.0);
            REQUIRE(const_cg1_view.exponent(2) == 9.0);
        }

        SECTION("exponent() const") {
            REQUIRE(std::as_const(cg1_view).exponent(0) == 7.0);
            REQUIRE(std::as_const(cg1_view).exponent(1) == 8.0);
            REQUIRE(std::as_const(cg1_view).exponent(2) == 9.0);

            REQUIRE(std::as_const(const_cg1_view).exponent(0) == 7.0);
            REQUIRE(std::as_const(const_cg1_view).exponent(1) == 8.0);
            REQUIRE(std::as_const(const_cg1_view).exponent(2) == 9.0);
        }

        SECTION("center()") {
            REQUIRE(cg1_view.center() == r1);
            REQUIRE(const_cg1_view.center() == r1);
        }

        SECTION("center() const") {
            REQUIRE(std::as_const(cg1_view).center() == r1);
            REQUIRE(std::as_const(const_cg1_view).center() == r1);
        }

        SECTION("operator[]") {
            // Correct values?
            REQUIRE(cg1_view[0] == prim_type(4.0, 7.0, r1));
            REQUIRE(cg1_view[1] == prim_type(5.0, 8.0, r1));
            REQUIRE(cg1_view[2] == prim_type(6.0, 9.0, r1));

            // Are aliases?
            REQUIRE(&cg1_view[0].coefficient() == &cg1[0].coefficient());
            REQUIRE(&cg1_view[0].exponent() == &cg1[0].exponent());
            REQUIRE(&cg1_view[1].coefficient() == &cg1[1].coefficient());
            REQUIRE(&cg1_view[1].exponent() == &cg1[1].exponent());
            REQUIRE(&cg1_view[2].coefficient() == &cg1[2].coefficient());
            REQUIRE(&cg1_view[2].exponent() == &cg1[2].exponent());
            REQUIRE(&cg1_view[0].center().x() == &cg1[0].center().x());
            REQUIRE(&cg1_view[0].center().y() == &cg1[0].center().y());
            REQUIRE(&cg1_view[0].center().z() == &cg1[0].center().z());

            // Correct values?
            REQUIRE(const_cg1_view[0] == prim_type(4.0, 7.0, r1));
            REQUIRE(const_cg1_view[1] == prim_type(5.0, 8.0, r1));
            REQUIRE(const_cg1_view[2] == prim_type(6.0, 9.0, r1));

            // Are aliases?
            REQUIRE(&const_cg1_view[0].coefficient() == &cg1[0].coefficient());
            REQUIRE(&const_cg1_view[0].exponent() == &cg1[0].exponent());
            REQUIRE(&const_cg1_view[1].coefficient() == &cg1[1].coefficient());
            REQUIRE(&const_cg1_view[1].exponent() == &cg1[1].exponent());
            REQUIRE(&const_cg1_view[2].coefficient() == &cg1[2].coefficient());
            REQUIRE(&const_cg1_view[2].exponent() == &cg1[2].exponent());
            REQUIRE(&const_cg1_view[0].center().x() == &cg1[0].center().x());
            REQUIRE(&const_cg1_view[0].center().y() == &cg1[0].center().y());
            REQUIRE(&const_cg1_view[0].center().z() == &cg1[0].center().z());
        }

        SECTION("operator[]() const") {
            REQUIRE(std::as_const(cg1_view)[0] == prim_type(4.0, 7.0, r1));
            REQUIRE(std::as_const(cg1_view)[1] == prim_type(5.0, 8.0, r1));
            REQUIRE(std::as_const(cg1_view)[2] == prim_type(6.0, 9.0, r1));

            // Are aliases?
            const auto& ccg1_view = std::as_const(cg1_view);
            REQUIRE(&ccg1_view[0].coefficient() == &cg1[0].coefficient());
            REQUIRE(&ccg1_view[0].exponent() == &cg1[0].exponent());
            REQUIRE(&ccg1_view[1].coefficient() == &cg1[1].coefficient());
            REQUIRE(&ccg1_view[1].exponent() == &cg1[1].exponent());
            REQUIRE(&ccg1_view[2].coefficient() == &cg1[2].coefficient());
            REQUIRE(&ccg1_view[2].exponent() == &cg1[2].exponent());
            REQUIRE(&ccg1_view[0].center().x() == &cg1[0].center().x());
            REQUIRE(&ccg1_view[0].center().y() == &cg1[0].center().y());
            REQUIRE(&ccg1_view[0].center().z() == &cg1[0].center().z());

            // Correct values?
            REQUIRE(std::as_const(const_cg1_view)[0] ==
                    prim_type(4.0, 7.0, r1));
            REQUIRE(std::as_const(const_cg1_view)[1] ==
                    prim_type(5.0, 8.0, r1));
            REQUIRE(std::as_const(const_cg1_view)[2] ==
                    prim_type(6.0, 9.0, r1));

            // Are aliases?
            const auto& cconst_cg1_view = std::as_const(const_cg1_view);
            REQUIRE(&cconst_cg1_view[0].coefficient() == &cg1[0].coefficient());
            REQUIRE(&cconst_cg1_view[0].exponent() == &cg1[0].exponent());
            REQUIRE(&cconst_cg1_view[1].coefficient() == &cg1[1].coefficient());
            REQUIRE(&cconst_cg1_view[1].exponent() == &cg1[1].exponent());
            REQUIRE(&cconst_cg1_view[2].coefficient() == &cg1[2].coefficient());
            REQUIRE(&cconst_cg1_view[2].exponent() == &cg1[2].exponent());
            REQUIRE(&cconst_cg1_view[0].center().x() == &cg1[0].center().x());
            REQUIRE(&cconst_cg1_view[0].center().y() == &cg1[0].center().y());
            REQUIRE(&cconst_cg1_view[0].center().z() == &cg1[0].center().z());
        }
    }

    SECTION("utility") {
        SECTION("operator==") {
            // default v default
            REQUIRE(cg0_view == view_type{});
            REQUIRE(const_cg0_view == const_view{});

            // default v non-default
            REQUIRE_FALSE(cg0_view == cg1_view);
            REQUIRE_FALSE(const_cg0_view == const_cg1_view);

            // non-default: same value
            auto& c0 = cg1[0].coefficient();
            auto& e0 = cg1[0].exponent();
            REQUIRE(cg1_view == view_type(3, c0, e0, r1));
            REQUIRE(const_cg1_view == const_view(3, c0, e0, r1));

            // non-default: different number of primitives
            REQUIRE_FALSE(cg1_view == view_type(2, c0, e0, r1));
            REQUIRE_FALSE(const_cg1_view == const_view(2, c0, e0, r1));

            // non-default: different coefficients
            REQUIRE_FALSE(cg1_view == view_type(3, e0, e0, r1));
            REQUIRE_FALSE(const_cg1_view == const_view(3, e0, e0, r1));

            // non-default: different exponents
            REQUIRE_FALSE(cg1_view == view_type(3, c0, c0, r1));
            REQUIRE_FALSE(const_cg1_view == const_view(3, c0, c0, r1));

            // non-default: different center
            REQUIRE_FALSE(cg1_view == view_type(3, c0, e0, r0));
            REQUIRE_FALSE(const_cg1_view == const_view(3, c0, e0, r0));
        }
    }
}
