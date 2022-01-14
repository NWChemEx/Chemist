#include "test_tensor.hpp"

using namespace chemist::tensor;

/* Testing Strategy:
 *
 * The actual TensorWrapper class is a pass through to the PIMPL in many
 * circumstances. For these unit tests we assume that the PIMPLs work and are
 * thoroughly tested. Thus for functions which are just pass throughs, we simply
 * need to ensure that arguments and returns are forwarded correctly.
 */

using scalar_traits  = backends::TiledArrayTraits<field::Scalar>;
using scalar_variant = typename scalar_traits::variant_type;

TEMPLATE_LIST_TEST_CASE("TensorWrapper<Scalar>", "", scalar_variant) {
    using field_type   = field::Scalar;
    using TWrapper     = TensorWrapper<field_type>;
    using shape_type   = typename TWrapper::shape_type;
    using extents_type = typename TWrapper::extents_type;
    using t_type       = TestType;

    // Assumed different type than the one returned by default_allocator
    using other_alloc = SingleElementTiles<field_type>;

    auto ta_tensors = testing::get_tensors<t_type>();
    t_type vec_data = ta_tensors.at("vector");
    t_type mat_data = ta_tensors.at("matrix");
    t_type t3_data  = ta_tensors.at("tensor");

    TWrapper defaulted;

    TWrapper vec(vec_data);
    TWrapper mat(mat_data);
    TWrapper t3(t3_data);

    auto default_alloc = default_allocator<field_type>();
    auto& world        = default_alloc->runtime();

    SECTION("CTors") {
        SECTION("Default") {
            REQUIRE(defaulted.rank() == 0);
            REQUIRE(defaulted.extents() == extents_type{});
            REQUIRE(defaulted.size() == 0);
        }

        SECTION("Allocator") {
            const auto* pa = &(*default_alloc);
            TWrapper t(std::move(default_alloc));
            REQUIRE(&t.allocator() == pa);
        }

        SECTION("Wrapping CTor") {
            REQUIRE(vec.rank() == 1);
            REQUIRE(vec.extents() == extents_type{3});
            REQUIRE(vec.size() == 3);
            REQUIRE(vec.allocator().is_equal(*default_alloc));
            REQUIRE(mat.rank() == 2);
            REQUIRE(mat.extents() == extents_type{2, 2});
            REQUIRE(mat.size() == 4);
            REQUIRE(mat.allocator().is_equal(*default_alloc));
            REQUIRE(t3.rank() == 3);
            REQUIRE(t3.extents() == extents_type{2, 2, 2});
            REQUIRE(t3.size() == 8);
            REQUIRE(t3.allocator().is_equal(*default_alloc));

            SECTION("Can set allocator") {
                auto palloc = &(*default_alloc);
                TWrapper t(vec_data, std::move(default_alloc));
                REQUIRE(&t.allocator() == palloc);
            }

            SECTION("Retiles if necessary") {
                auto palloc = std::make_unique<other_alloc>();
                auto tr     = palloc->make_tiled_range(std::vector{3ul});
                t_type corr_data(world, tr, {1.0, 2.0, 3.0});
                TWrapper corr(std::move(corr_data), std::move(palloc));
                auto palloc2 = std::make_unique<other_alloc>(world);
                TWrapper retiled(vec_data, std::move(palloc2));
                REQUIRE(retiled == corr);
            }
        }

        SECTION("Shape") {
            auto new_shape = std::make_unique<shape_type>(extents_type{2, 2});
            auto pshape    = &(*new_shape);
            auto palloc    = &(*default_alloc);
            TWrapper t(mat_data, std::move(new_shape),
                       std::move(default_alloc));
            REQUIRE(t == mat);
            // Make sure they're not copied
            REQUIRE(&t.allocator() == palloc);
            REQUIRE(&t.shape() == pshape);
        }

        SECTION("Copy") {
            TWrapper copied(vec);
            REQUIRE(copied.rank() == 1);
            REQUIRE(copied.extents() == extents_type{3});
            REQUIRE(copied.allocator().is_equal(vec.allocator()));
        }

        SECTION("Move") {
            const auto* pa = &(vec.allocator());
            TWrapper moved(std::move(vec));
            REQUIRE(moved.rank() == 1);
            REQUIRE(moved.extents() == extents_type{3});
            REQUIRE(&moved.allocator() == pa);
        }

        SECTION("Copy assignment") {
            TWrapper copied;
            auto pcopied = &(copied = vec);
            REQUIRE(pcopied == &copied);
            REQUIRE(copied.rank() == 1);
            REQUIRE(copied.extents() == extents_type{3});
            REQUIRE(copied.allocator().is_equal(vec.allocator()));
        }

        SECTION("Move assignment") {
            TWrapper moved;
            const auto* pa = &(vec.allocator());
            auto pmoved    = &(moved = std::move(vec));
            REQUIRE(pmoved == &moved);
            REQUIRE(moved.rank() == 1);
            REQUIRE(moved.extents() == extents_type{3});
            REQUIRE(&moved.allocator() == pa);
        }
    }

    SECTION("allocator") {
        REQUIRE_THROWS_AS(defaulted.allocator(), std::runtime_error);
        REQUIRE(vec.allocator().is_equal(*default_alloc));
        REQUIRE(mat.allocator().is_equal(*default_alloc));
        REQUIRE(t3.allocator().is_equal(*default_alloc));
    }

    SECTION("reallocate") {
        auto new_p     = std::make_unique<other_alloc>(world);
        const auto* pa = &(*new_p);

        SECTION("Default") {
            defaulted.reallocate(std::move(new_p));
            REQUIRE(&defaulted.allocator() == pa);
        }

        SECTION("Non-default") {
            TWrapper corr(vec_data, new_p->clone());
            vec.reallocate(std::move(new_p));
            REQUIRE(vec == corr);
            REQUIRE(&vec.allocator() == pa);
        }
    }

    SECTION("make_annotation") {
        REQUIRE(defaulted.make_annotation() == "");
        REQUIRE(vec.make_annotation() == "i0");
        REQUIRE(mat.make_annotation("j") == "j0,j1");
        REQUIRE(t3.make_annotation() == "i0,i1,i2");
    }

    SECTION("rank()") {
        REQUIRE(defaulted.rank() == 0);
        REQUIRE(vec.rank() == 1);
        REQUIRE(mat.rank() == 2);
        REQUIRE(t3.rank() == 3);
    }

    SECTION("extents()") {
        REQUIRE(defaulted.extents() == extents_type{});
        REQUIRE(vec.extents() == extents_type{3});
        REQUIRE(mat.extents() == extents_type{2, 2});
        REQUIRE(t3.extents() == extents_type{2, 2, 2});
    }

    SECTION("shape()") {
        REQUIRE(vec.shape() == shape_type(vec.extents()));
        REQUIRE(mat.shape() == shape_type(mat.extents()));
        REQUIRE(t3.shape() == shape_type(t3.extents()));
    }

    SECTION("slice()") {
        SECTION("Vector") {
            TWrapper corr(t_type(world, {1.0, 2.0}));
            auto slice = vec.slice({0ul}, {2ul});
            REQUIRE(slice == corr);
        }
        SECTION("Matrix") {
            TWrapper corr(t_type(world, {{2.0}}));
            auto slice = mat.slice({0ul, 1ul}, {1ul, 2ul});
            REQUIRE(slice == corr);
        }
        SECTION("Tensor") {
            TWrapper corr(t_type(world, {{{2.0}, {4.0}}, {{6.0}, {8.0}}}));
            auto slice = t3.slice({0ul, 0ul, 1ul}, {2ul, 2ul, 2ul});
            REQUIRE(slice == corr);
        }
        SECTION("Different allocator") {
            auto p = std::make_unique<other_alloc>(world);
            TWrapper corr(t_type(world, {1.0, 2.0}), p->clone());
            auto slice = vec.slice({0ul}, {2ul}, std::move(p));
            REQUIRE(slice == corr);
        }
    }

    SECTION("reshape()") {
        SECTION("Incorrect shape") {
            auto p = std::make_unique<shape_type>(extents_type{2, 3});
            REQUIRE_THROWS_AS(vec.reshape(std::move(p)), std::runtime_error);
        }
        SECTION("Vector to matrix") {
            auto p = std::make_unique<shape_type>(extents_type{1, 3});
            TWrapper corr(t_type(world, {{1.0, 2.0, 3.0}}));
            auto rv = vec.reshape(std::move(p));
            REQUIRE(rv == corr);
        }
        SECTION("Matrix to vector") {
            auto p = std::make_unique<shape_type>(extents_type{4});
            TWrapper corr(t_type(world, {1.0, 2.0, 3.0, 4.0}));
            auto rv = mat.reshape(std::move(p));
            REQUIRE(rv == corr);
        }
        SECTION("tensor to matrix") {
            TWrapper corr(
              t_type(world, {{1.0, 2.0, 3.0, 4.0}, {5.0, 6.0, 7.0, 8.0}}));
            auto p  = std::make_unique<shape_type>(extents_type{2, 4});
            auto rv = t3.reshape(std::move(p));
            REQUIRE(rv == corr);
        }
    }

    SECTION("operator()") {
        // Basically just testing that it compiles, real test happens in
        // labeled_tensor_wrapper.cpp
        REQUIRE_NOTHROW(vec("i"));
        REQUIRE_NOTHROW(mat("i,j"));
        REQUIRE_NOTHROW(t3("i,j,k"));
    }

    SECTION("operator() const") {
        // Basically just testing that it compiles, real test happens in
        // labeled_tensor_wrapper.cpp
        REQUIRE_NOTHROW(std::as_const(vec)("i"));
        REQUIRE_NOTHROW(std::as_const(mat)("i,j"));
        REQUIRE_NOTHROW(std::as_const(t3)("i,j,k"));
    }

    SECTION("get()") {
        using chemist::ta_helpers::allclose;
        SECTION("Vector") { REQUIRE(allclose(vec.get<t_type>(), vec_data)); }
        SECTION("Matrix") { REQUIRE(allclose(mat.get<t_type>(), mat_data)); }
        SECTION("Rank 3 Tenosr") {
            REQUIRE(allclose(t3.get<t_type>(), t3_data));
        }
    }

    SECTION("get() const") {
        using chemist::ta_helpers::allclose;
        SECTION("Vector") {
            REQUIRE(allclose(std::as_const(vec).get<t_type>(), vec_data));
        }
        SECTION("Matrix") {
            REQUIRE(allclose(std::as_const(mat).get<t_type>(), mat_data));
        }
        SECTION("Rank 3 Tenosr") {
            REQUIRE(allclose(std::as_const(t3).get<t_type>(), t3_data));
        }
    }

    SECTION("print") {
        std::stringstream ss;
        auto pss = &(vec.print(ss));
        SECTION("Returns ss for chaining") { REQUIRE(pss == &ss); }
        SECTION("Value") {
            std::string corr = "0: [ [0], [3] ) { 1 2 3 }\n";
            REQUIRE(corr == ss.str());
        }
    }

    SECTION("operator<<") {
        std::stringstream ss;
        auto pss = &(ss << vec);
        SECTION("Returns ss for chaining") { REQUIRE(pss == &ss); }
        SECTION("Value") {
            std::string corr = "0: [ [0], [3] ) { 1 2 3 }\n";
            REQUIRE(corr == ss.str());
        }
    }

    SECTION("hash") {
        TWrapper other_vec(vec_data);
        REQUIRE(pluginplay::hash_objects(other_vec) ==
                pluginplay::hash_objects(vec));
        REQUIRE(pluginplay::hash_objects(vec) != pluginplay::hash_objects(mat));
    }

    SECTION("comparisons") {
        SECTION("Are same") {
            TWrapper other_vec(vec_data);
            REQUIRE(vec == other_vec);
            REQUIRE_FALSE(vec != other_vec);
        }
        SECTION("Different") {
            REQUIRE_FALSE(vec == mat);
            REQUIRE(vec != mat);
        }
    }

    /* This bug was found by Jonathan Waldrop. What was happening was that if
       you default constructed a TensorWrapper instance A, A has no
       allocator. When you then assigned to A (from a filled instance B), A
       got the values of B, but no allocator. When you then performed an
       operation which requires usage of the allocator in A (such as slicing,
       which used to clone A's allocator and give it to the slice) you got a
       segfault.
     */
    SECTION("Slicing after default construction") {
        TWrapper A;
        A("i,j")        = mat("i,j");
        auto slice_of_A = A.slice({0ul, 1ul}, {1ul, 2ul});
        TWrapper corr(t_type(world, {{2.0}}));
        REQUIRE(slice_of_A == corr);
    }
}
