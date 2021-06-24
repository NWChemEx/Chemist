#include "libchemist/orbital_space/derived_space.hpp"
#include <catch2/catch.hpp>

/* For testing purposes we assume:
 *
 * - The actual identity/state of the from space does not impact the
 *   DerivedSpace class aside from needing to have a slot in the ctor and be
 *   accessible through the DerivedSpace members. i.e. as long as that type is
 *   tested it'll work here too.
 *
 * - The base class more-or-less passes through the DerivedSpace class. i.e.,
 *   if the base class works, it'll work when used with DerivedSpace too.
 *
 *   - The caveats are that we need to make sure it's accounted for in the ctor,
 *     hash, and comparison operations.
 */

using namespace libchemist;
using namespace libchemist::orbital_space;


TEST_CASE("DerivedSpace") {
    using scalar_type = double;
    using space_type  = DerivedSpaceD;
    using tensor_type = libchemist::type::tensor<scalar_type>;
    using from_space  = AOSpaceD;
    using base_space  = BaseSpace;

    SECTION("Typedefs") {
        SECTION("transform_type") {
            using transform_type = typename space_type::transform_type;
            STATIC_REQUIRE(std::is_same_v<transform_type, tensor_type>);
        }
        SECTION("from_space_type") {
            using from_space_type = typename space_type::from_space_type;
            using corr = from_space;
            STATIC_REQUIRE(std::is_same_v<from_space_type, corr>);
        }
        SECTION("from_space_ptr") {
            using from_space_ptr = typename space_type::from_space_ptr;
            using corr = std::shared_ptr<const from_space>;
            STATIC_REQUIRE(std::is_same_v<from_space_ptr, corr>);
        }
        SECTION("size_type") {
            using size_type = typename space_type::size_type;
            using corr = typename base_space::size_type;
            STATIC_REQUIRE(std::is_same_v<size_type, corr>);
        }
    }
}

TEST_CASE("IndDerivedSpace") {
    using scalar_type = double;
    using space_type  = IndDerivedSpaceD;
    using tensor_type = libchemist::type::tensor<scalar_type>;
    using from_space  = DepAOSpaceD;
    using base_space  = BaseSpace;

    SECTION("Typedefs") {
        SECTION("transform_type") {
            using transform_type = typename space_type::transform_type;
            STATIC_REQUIRE(std::is_same_v<transform_type, tensor_type>);
        }
        SECTION("from_space_type") {
            using from_space_type = typename space_type::from_space_type;
            using corr = from_space;
            STATIC_REQUIRE(std::is_same_v<from_space_type, corr>);
        }
        SECTION("from_space_ptr") {
            using from_space_ptr = typename space_type::from_space_ptr;
            using corr = std::shared_ptr<const from_space>;
            STATIC_REQUIRE(std::is_same_v<from_space_ptr, corr>);
        }
        SECTION("size_type") {
            using size_type = typename space_type::size_type;
            using corr = typename base_space::size_type;
            STATIC_REQUIRE(std::is_same_v<size_type, corr>);
        }
    }
}

TEST_CASE("DepDerivedSpace") {
    using scalar_type = double;
    using space_type  = DepDerivedSpaceD;
    using tensor_type = libchemist::type::tensor_of_tensors<scalar_type>;
    using from_space  = DepAOSpaceD;
    using base_space  = DependentSpace;

    SECTION("Typedefs") {
        SECTION("transform_type") {
            using transform_type = typename space_type::transform_type;
            STATIC_REQUIRE(std::is_same_v<transform_type, tensor_type>);
        }
        SECTION("from_space_type") {
            using from_space_type = typename space_type::from_space_type;
            using corr = from_space;
            STATIC_REQUIRE(std::is_same_v<from_space_type, corr>);
        }
        SECTION("from_space_ptr") {
            using from_space_ptr = typename space_type::from_space_ptr;
            using corr = std::shared_ptr<const from_space>;
            STATIC_REQUIRE(std::is_same_v<from_space_ptr, corr>);
        }
        SECTION("size_type") {
            using size_type = typename space_type::size_type;
            using corr = typename base_space::size_type;
            STATIC_REQUIRE(std::is_same_v<size_type, corr>);
        }
    }
}
