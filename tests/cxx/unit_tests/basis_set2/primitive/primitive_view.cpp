#include <catch2/catch.hpp>
#include <chemist/basis_set2/primitive/primitive_view.hpp>

using namespace chemist::basis_set;

TEMPLATE_TEST_CASE("PrimitiveView", "", float, double) {
    using prim_type   = Primitive<TestType>;
    using center_type = typename prim_type::center_type;
    using view_type   = PrimitiveView<prim_type>;
    using const_view  = PrimitiveView<const prim_type>;

    center_type r0(1.0, 2.0, 3.0);
    prim_type defaulted;
    prim_type values(4.0, 5.0, r0);

    view_type defaulted_view(defaulted);
    view_type values_view(values);

    const_view const_defaulted_view(defaulted);
    const_view const_values_view(values);

    SECTION("Ctors and assignment") {
        SECTION("coef, exp, x, y, z") {
            view_type p(values.coefficient(), values.exponent(),
                        values.center().x(), values.center().y(),
                        values.center().z());

            // Check coefficient value and ensure an alias
            REQUIRE(p.coefficient() == 4.0);
            REQUIRE(&p.coefficient() == &values.coefficient());

            // Check exponent value and ensure an alias
            REQUIRE(p.exponent() == 5.0);
            REQUIRE(&p.exponent() == &values.exponent());

            // Check center value
            REQUIRE(p.center() == r0);

            // Ensure coordinates are aliases
            REQUIRE(&p.center().x() == &values.center().x());
            REQUIRE(&p.center().y() == &values.center().y());
            REQUIRE(&p.center().z() == &values.center().z());

            const_view cp(values.coefficient(), values.exponent(),
                          values.center().x(), values.center().y(),
                          values.center().z());

            // Check coefficient value and ensure an alias
            REQUIRE(cp.coefficient() == 4.0);
            REQUIRE(&cp.coefficient() == &values.coefficient());

            // Check exponent value and ensure an alias
            REQUIRE(cp.exponent() == 5.0);
            REQUIRE(&cp.exponent() == &values.exponent());

            // Check center value
            REQUIRE(cp.center() == r0);

            // Ensure coordinates are aliases
            REQUIRE(&cp.center().x() == &values.center().x());
            REQUIRE(&cp.center().y() == &values.center().y());
            REQUIRE(&cp.center().z() == &values.center().z());
        }

        SECTION("coef, exp, r0") {
            view_type p(values.coefficient(), values.exponent(),
                        values.center());

            // Check coefficient value and ensure an alias
            REQUIRE(p.coefficient() == 4.0);
            REQUIRE(&p.coefficient() == &values.coefficient());

            // Check exponent value and ensure an alias
            REQUIRE(p.exponent() == 5.0);
            REQUIRE(&p.exponent() == &values.exponent());

            // Check center value
            REQUIRE(p.center() == r0);

            // Ensure coordinates are aliases
            REQUIRE(&p.center().x() == &values.center().x());
            REQUIRE(&p.center().y() == &values.center().y());
            REQUIRE(&p.center().z() == &values.center().z());

            const_view cp(values.coefficient(), values.exponent(),
                          values.center());

            // Check coefficient value and ensure an alias
            REQUIRE(cp.coefficient() == 4.0);
            REQUIRE(&cp.coefficient() == &values.coefficient());

            // Check exponent value and ensure an alias
            REQUIRE(cp.exponent() == 5.0);
            REQUIRE(&cp.exponent() == &values.exponent());

            // Check center value
            REQUIRE(cp.center() == r0);

            // Ensure coordinates are aliases
            REQUIRE(&cp.center().x() == &values.center().x());
            REQUIRE(&cp.center().y() == &values.center().y());
            REQUIRE(&cp.center().z() == &values.center().z());
        }

        SECTION("primitive") {
            REQUIRE(defaulted_view.is_null());

            // Check coefficient value and ensure an alias
            REQUIRE(values_view.coefficient() == 4.0);
            REQUIRE(&values_view.coefficient() == &values.coefficient());

            // Check exponent value and ensure an alias
            REQUIRE(values_view.exponent() == 5.0);
            REQUIRE(&values_view.exponent() == &values.exponent());

            // Check center value
            REQUIRE(values_view.center() == r0);

            // Ensure coordinates are aliases
            REQUIRE(&values_view.center().x() == &values.center().x());
            REQUIRE(&values_view.center().y() == &values.center().y());
            REQUIRE(&values_view.center().z() == &values.center().z());

            REQUIRE(const_defaulted_view.is_null());

            // Check coefficient value and ensure an alias
            REQUIRE(const_values_view.coefficient() == 4.0);
            REQUIRE(&const_values_view.coefficient() == &values.coefficient());

            // Check exponent value and ensure an alias
            REQUIRE(const_values_view.exponent() == 5.0);
            REQUIRE(&const_values_view.exponent() == &values.exponent());

            // Check center value
            REQUIRE(const_values_view.center() == r0);

            // Ensure coordinates are aliases
            REQUIRE(&const_values_view.center().x() == &values.center().x());
            REQUIRE(&const_values_view.center().y() == &values.center().y());
            REQUIRE(&const_values_view.center().z() == &values.center().z());
        }
    }
}
