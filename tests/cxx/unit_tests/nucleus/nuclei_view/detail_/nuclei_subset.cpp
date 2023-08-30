#include <catch2/catch.hpp>
#include <chemist/nucleus/nuclei_view/detail_/nuclei_subset.hpp>

using namespace chemist::detail_;

TEST_CASE("NucleiSubset") {
    using nuclei_type = chemist::Nuclei;
    using pimpl_type  = NucleiSubset<nuclei_type>;
    using const_pimpl = NucleiSubset<const nuclei_type>;

    // Create some Nucleus objects. Objects do not need to be physical, just
    // distinct.
    using nucleus_type = typename pimpl_type::value_type;

    nucleus_type h0("H", 1ul, 0.0, 1.0, 2.0, 3.0, 4.0);
    nucleus_type h1("H", 1ul, 0.0, 5.0, 6.0, 7.0, 8.0);
    nucleus_type h2("H", 1ul, 0.0, 9.0, 8.0, 7.0, 6.0);

    // Create a Nuclei object
    auto pnukes = std::make_shared<nuclei_type>(nuclei_type{h0, h1, h2});

    // Create pimpl_type objects
    using size_type = typename pimpl_type::size_type;
    pimpl_type defaulted;
    const_pimpl const_defaulted;

    std::vector<size_type> indices{1, 2};
    pimpl_type has_values(pnukes, indices.begin(), indices.end());
    const_pimpl const_has_values(pnukes, indices.begin(), indices.end());

    SECTION("Ctors") {
        SECTION("Default") {
            REQUIRE(defaulted.size() == 0);
            REQUIRE(defaulted.is_null());

            REQUIRE(const_defaulted.size() == 0);
            REQUIRE(const_defaulted.is_null());
        }

        SECTION("value") {
            REQUIRE_FALSE(has_values.is_null());
            REQUIRE(has_values.size() == 2);
            REQUIRE(has_values.get_nuke(0) == (*pnukes)[1]);
            REQUIRE(has_values.get_nuke(1) == (*pnukes)[2]);

            REQUIRE_FALSE(const_has_values.is_null());
            REQUIRE(const_has_values.size() == 2);
            REQUIRE(const_has_values.get_nuke(0) == (*pnukes)[1]);
            REQUIRE(const_has_values.get_nuke(1) == (*pnukes)[2]);
        }

        SECTION("copy") {
            pimpl_type defaulted_copy(defaulted);
            REQUIRE(defaulted_copy.size() == 0);
            REQUIRE(defaulted.is_null());

            const_pimpl const_defaulted_copy(const_defaulted);
            REQUIRE(const_defaulted_copy.size() == 0);
            REQUIRE(const_defaulted.is_null());

            pimpl_type has_values_copy(has_values);
            REQUIRE_FALSE(has_values_copy.is_null());
            REQUIRE(has_values_copy.size() == 2);
            REQUIRE(has_values_copy.get_nuke(0) == (*pnukes)[1]);
            REQUIRE(has_values_copy.get_nuke(1) == (*pnukes)[2]);

            const_pimpl const_has_values_copy(const_has_values);
            REQUIRE_FALSE(const_has_values_copy.is_null());
            REQUIRE(const_has_values_copy.size() == 2);
            REQUIRE(const_has_values_copy.get_nuke(0) == (*pnukes)[1]);
            REQUIRE(const_has_values_copy.get_nuke(1) == (*pnukes)[2]);
        }
    }

    SECTION("clone") {
        auto pdefaulted = defaulted.clone();
        REQUIRE(pdefaulted->size() == 0);

        auto pconst_defaulted = const_defaulted.clone();
        REQUIRE(pconst_defaulted->size() == 0);

        auto phas_value = has_values.clone();
        REQUIRE(phas_value->size() == 2);
        REQUIRE(phas_value->get_nuke(0) == (*pnukes)[1]);
        REQUIRE(phas_value->get_nuke(1) == (*pnukes)[2]);

        auto pconst_has_value = const_has_values.clone();
        REQUIRE(pconst_has_value->size() == 2);
        REQUIRE(pconst_has_value->get_nuke(0) == (*pnukes)[1]);
        REQUIRE(pconst_has_value->get_nuke(1) == (*pnukes)[2]);
    }

    SECTION("get_nuke") {
        REQUIRE(has_values.get_nuke(0) == (*pnukes)[1]);
        REQUIRE(has_values.get_nuke(1) == (*pnukes)[2]);

        REQUIRE(const_has_values.get_nuke(0) == (*pnukes)[1]);
        REQUIRE(const_has_values.get_nuke(1) == (*pnukes)[2]);
    }

    SECTION("get_nuke()const") {
        REQUIRE(std::as_const(has_values).get_nuke(0) == (*pnukes)[1]);
        REQUIRE(std::as_const(has_values).get_nuke(1) == (*pnukes)[2]);

        REQUIRE(std::as_const(const_has_values).get_nuke(0) == (*pnukes)[1]);
        REQUIRE(std::as_const(const_has_values).get_nuke(1) == (*pnukes)[2]);
    }

    SECTION("size") {
        REQUIRE(defaulted.size() == 0);
        REQUIRE(const_defaulted.size() == 0);

        REQUIRE(has_values.size() == 2);
        REQUIRE(const_has_values.size() == 2);
    }

    SECTION("operator==") {
        // Defaulted == defaulted
        REQUIRE(defaulted == pimpl_type{});

        // Defaulted != non-default
        REQUIRE_FALSE(defaulted == has_values);

        // Same everything
        pimpl_type has_values2(pnukes, indices.begin(), indices.end());
        REQUIRE(has_values == has_values2);

        // Different instance of same supersystem
        auto pnukes1 = std::make_shared<nuclei_type>(nuclei_type{h0, h1, h2});
        pimpl_type has_values3(pnukes1, indices.begin(), indices.end());
        REQUIRE(has_values == has_values3);

        // Different supersystem (same atoms)
        nucleus_type h3("H", 1ul, 0.0, 1.1, 2.2, 3.3, 4.4);
        auto pnukes2 = std::make_shared<nuclei_type>(nuclei_type{h3, h1, h2});
        pimpl_type diff_ss(pnukes2, indices.begin(), indices.end());
        REQUIRE_FALSE(has_values == diff_ss);

        // Different atoms
        std::vector<size_type> indices2{0, 1};
        pimpl_type diff_atoms(pnukes, indices2.begin(), indices2.end());
        REQUIRE_FALSE(has_values == diff_atoms);
    }
}