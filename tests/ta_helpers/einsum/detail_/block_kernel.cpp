#include <catch2/catch.hpp>
#include <libchemist/ta_helpers/einsum/einsum.hpp>
#include <libchemist/ta_helpers/ta_helpers.hpp>

using namespace libchemist::ta_helpers::einsum::detail_;
using ranges_type = libchemist::ta_helpers::einsum::types::assoc_range;

TEST_CASE("block_kernel") {
    SECTION("Vector-vector") {
        /*
         * i = i * i is the only einsum between two vectors with one index
         */
        SECTION("One index") {
            ranges_type ranges{{"i", {0, 2}}};

            IndexMap im("i", "i", "i");
            std::array a_data{2.0, 3.0};
            std::array b_data{4.0, 5.0};
            std::array c_data{8.0, 15.0};

            auto&& [orange, lrange, rrange] = im.select(ranges);
            TA::TensorD a(make_range(lrange), a_data.data());
            TA::TensorD b(make_range(rrange), b_data.data());
            TA::TensorD corr(make_range(orange), c_data.data());

            auto result = block_kernel(im, ranges, a, b);
            REQUIRE((result - corr).abs_max() < 1E-6);
        }

        /*
         *  With two indices we can combine two vectors and get a vector back:
         *
         *  i = i * j (same as  j = j * i)
         *  i = j * i (same as  j = i * j)
         *
         *  or we can get a matrix back:
         *
         *  ij = i * j (same as ji = j * i)
         *  ij = j * i (same as ji = i * j)
         */
        SECTION("Two indices") {
            ranges_type ranges{{"i", {0, 2}},{"j", {1, 4}}};

            SECTION("i = i * j") {
                IndexMap im("i", "i", "j");
                std::array a_data{2.0, 3.0};
                std::array b_data{4.0, 5.0, 6.0};
                std::array c_data{30.0, 45.0};

                auto&& [orange, lrange, rrange] = im.select(ranges);
                TA::TensorD a(make_range(lrange), a_data.data());
                TA::TensorD b(make_range(rrange), b_data.data());
                TA::TensorD corr(make_range(orange), c_data.data());

                auto result = block_kernel(im, ranges, a, b);
                REQUIRE((result - corr).abs_max() < 1E-6);
            }

            SECTION("i = j * i") {
                IndexMap im("i", "j", "i");
                std::array b_data{2.0, 3.0};
                std::array a_data{4.0, 5.0, 6.0};
                std::array c_data{30.0, 45.0};

                auto&& [orange, lrange, rrange] = im.select(ranges);
                TA::TensorD a(make_range(lrange), a_data.data());
                TA::TensorD b(make_range(rrange), b_data.data());
                TA::TensorD corr(make_range(orange), c_data.data());

                auto result = block_kernel(im, ranges, a, b);
                REQUIRE((result - corr).abs_max() < 1E-6);
            }

            SECTION("ij = i * j") {
                IndexMap im("i,j", "i", "j");
                std::array a_data{2.0, 3.0};
                std::array b_data{4.0, 5.0, 6.0};
                std::array c_data{8.0, 10.0, 12.0, 12.0, 15.0, 18.0};

                auto&& [orange, lrange, rrange] = im.select(ranges);
                TA::TensorD a(make_range(lrange), a_data.data());
                TA::TensorD b(make_range(rrange), b_data.data());
                TA::TensorD corr(make_range(orange), c_data.data());

                auto result = block_kernel(im, ranges, a, b);
                REQUIRE((result - corr).abs_max() < 1E-6);
            }

            SECTION("ij = j * i") {
                IndexMap im("i,j", "j", "i");
                std::array b_data{2.0, 3.0};
                std::array a_data{4.0, 5.0, 6.0};
                std::array c_data{8.0, 10.0, 12.0, 12.0, 15.0, 18.0};

                auto&& [orange, lrange, rrange] = im.select(ranges);
                TA::TensorD a(make_range(lrange), a_data.data());
                TA::TensorD b(make_range(rrange), b_data.data());
                TA::TensorD corr(make_range(orange), c_data.data());

                auto result = block_kernel(im, ranges, a, b);
                REQUIRE((result - corr).abs_max() < 1E-6);
            }
        }
    }

    SECTION("Vector-matrix") {
        SECTION("One index") {
            ranges_type ranges{{"i", {0, 2}}};

            IndexMap im("i", "i", "i, i");
            std::array a_data{2.0, 3.0};
            std::array b_data{4.0, 5.0, 6.0, 7.0};
            std::array c_data{8.0, 21.0};

            auto&& [orange, lrange, rrange] = im.select(ranges);
            TA::TensorD a(make_range(lrange), a_data.data());
            TA::TensorD b(make_range(rrange), b_data.data());
            TA::TensorD corr(make_range(orange), c_data.data());

            auto result = block_kernel(im, ranges, a, b);
            REQUIRE((result - corr).abs_max() < 1E-6);
        }

        /* i = i * ij (same as j = j * ji)
         * i = i * ji (same as j = j * ij)
         * i = j * ii (same as j = i * jj)
         * i = j * ij (same as j = i * ji)
         * i = j * ji (same as j = i * ij)
         */
        SECTION("Two indices resulting in vector") {
            ranges_type ranges{{"i", {0, 2}},{"j", {1, 4}}};

            SECTION("i = i * ij") {
                IndexMap im("i", "i", "i, j");
                std::array a_data{2.0, 3.0};
                std::array b_data{4.0, 5.0, 6.0, 7.0, 8.0, 9.0};
                std::array c_data{30.0, 72.0};

                auto&& [orange, lrange, rrange] = im.select(ranges);
                TA::TensorD a(make_range(lrange), a_data.data());
                TA::TensorD b(make_range(rrange), b_data.data());
                TA::TensorD corr(make_range(orange), c_data.data());

                auto result = block_kernel(im, ranges, a, b);
                REQUIRE((result - corr).abs_max() < 1E-6);
            }

            SECTION("i = i * ji") {
                IndexMap im("i", "i", "j, i");
                std::array a_data{2.0, 3.0};
                std::array b_data{4.0, 7.0, 5.0, 8.0, 6.0, 9.0};
                std::array c_data{30.0, 72.0};

                auto&& [orange, lrange, rrange] = im.select(ranges);
                TA::TensorD a(make_range(lrange), a_data.data());
                TA::TensorD b(make_range(rrange), b_data.data());
                TA::TensorD corr(make_range(orange), c_data.data());

                auto result = block_kernel(im, ranges, a, b);
                REQUIRE((result - corr).abs_max() < 1E-6);
            }

            SECTION("i = j * ii") {
                IndexMap im("i", "j", "i, i");
                std::array a_data{2.0, 3.0, 4.0};
                std::array b_data{5.0, 6.0, 7.0, 8.0};
                std::array c_data{45.0, 72.0};

                auto&& [orange, lrange, rrange] = im.select(ranges);
                TA::TensorD a(make_range(lrange), a_data.data());
                TA::TensorD b(make_range(rrange), b_data.data());
                TA::TensorD corr(make_range(orange), c_data.data());

                auto result = block_kernel(im, ranges, a, b);
                REQUIRE((result - corr).abs_max() < 1E-6);
            }

            SECTION("i = j * ij") {
                IndexMap im("i", "j", "i, j");
                std::array a_data{2.0, 3.0, 4.0};
                std::array b_data{5.0, 6.0, 7.0, 8.0, 9.0, 10.0};
                std::array c_data{56.0, 83.0};

                auto&& [orange, lrange, rrange] = im.select(ranges);
                TA::TensorD a(make_range(lrange), a_data.data());
                TA::TensorD b(make_range(rrange), b_data.data());
                TA::TensorD corr(make_range(orange), c_data.data());

                auto result = block_kernel(im, ranges, a, b);
                REQUIRE((result - corr).abs_max() < 1E-6);
            }

            SECTION("i = j * ji") {
                IndexMap im("i", "j", "j, i");
                std::array a_data{2.0, 3.0, 4.0};
                std::array b_data{5.0, 8.0, 6.0, 9.0, 7.0, 10.0};
                std::array c_data{56.0, 83.0};

                auto&& [orange, lrange, rrange] = im.select(ranges);
                TA::TensorD a(make_range(lrange), a_data.data());
                TA::TensorD b(make_range(rrange), b_data.data());
                TA::TensorD corr(make_range(orange), c_data.data());

                auto result = block_kernel(im, ranges, a, b);
                REQUIRE((result - corr).abs_max() < 1E-6);
            }
        }

        /* ij = i * ij (same as ji = j * ji)
         * ij = i * ji (same as ji = j * ij)
         * ij = j * ii (same as ji = i * jj)
         * ij = j * ij (same as ji = i * ji)
         * ij = j * ji (same as ji = i * ij)
         */
        SECTION("Two indices resulting in matrix") {
            ranges_type ranges{{"i", {0, 2}},{"j", {1, 4}}};

            SECTION("ij = i * ij") {
                IndexMap im("i,j", "i", "i, j");
                std::array a_data{2.0, 3.0};
                std::array b_data{4.0, 5.0, 6.0, 7.0, 8.0, 9.0};
                std::array c_data{8.0, 10.0, 12.0, 21.0, 24.0, 27.0};

                auto&& [orange, lrange, rrange] = im.select(ranges);
                TA::TensorD a(make_range(lrange), a_data.data());
                TA::TensorD b(make_range(rrange), b_data.data());
                TA::TensorD corr(make_range(orange), c_data.data());

                auto result = block_kernel(im, ranges, a, b);
                REQUIRE((result - corr).abs_max() < 1E-6);
            }

            SECTION("ij = i * ji") {
                IndexMap im("i,j", "i", "j, i");
                std::array a_data{2.0, 3.0};
                std::array b_data{4.0, 7.0, 5.0, 8.0, 6.0, 9.0};
                std::array c_data{8.0, 10.0, 12.0, 21.0, 24.0, 27.0};

                auto&& [orange, lrange, rrange] = im.select(ranges);
                TA::TensorD a(make_range(lrange), a_data.data());
                TA::TensorD b(make_range(rrange), b_data.data());
                TA::TensorD corr(make_range(orange), c_data.data());

                auto result = block_kernel(im, ranges, a, b);
                REQUIRE((result - corr).abs_max() < 1E-6);
            }

            SECTION("ij = j * ii") {
                IndexMap im("i,j", "j", "i, i");
                std::array a_data{2.0, 3.0, 4.0};
                std::array b_data{5.0, 6.0, 7.0, 8.0};
                std::array c_data{10.0, 15.0, 20.0, 16.0, 24.0, 32.0};

                auto&& [orange, lrange, rrange] = im.select(ranges);
                TA::TensorD a(make_range(lrange), a_data.data());
                TA::TensorD b(make_range(rrange), b_data.data());
                TA::TensorD corr(make_range(orange), c_data.data());

                auto result = block_kernel(im, ranges, a, b);
                REQUIRE((result - corr).abs_max() < 1E-6);
            }

            SECTION("ij = j * ij") {
                IndexMap im("i,j", "j", "i, j");
                std::array a_data{2.0, 3.0, 4.0};
                std::array b_data{5.0, 6.0, 7.0, 8.0, 9.0, 10.0};
                std::array c_data{10.0, 18.0, 28.0, 16.0, 27.0, 40.0};

                auto&& [orange, lrange, rrange] = im.select(ranges);
                TA::TensorD a(make_range(lrange), a_data.data());
                TA::TensorD b(make_range(rrange), b_data.data());
                TA::TensorD corr(make_range(orange), c_data.data());

                auto result = block_kernel(im, ranges, a, b);
                REQUIRE((result - corr).abs_max() < 1E-6);
            }

            SECTION("ij = j * ji") {
                IndexMap im("i,j", "j", "j, i");
                std::array a_data{2.0, 3.0, 4.0};
                std::array b_data{5.0, 8.0, 6.0, 9.0, 7.0, 10.0};
                std::array c_data{10.0, 18.0, 28.0, 16.0, 27.0, 40.0};

                auto&& [orange, lrange, rrange] = im.select(ranges);
                TA::TensorD a(make_range(lrange), a_data.data());
                TA::TensorD b(make_range(rrange), b_data.data());
                TA::TensorD corr(make_range(orange), c_data.data());

                auto result = block_kernel(im, ranges, a, b);
                REQUIRE((result - corr).abs_max() < 1E-6);
            }
        }

        SECTION("Three indices resulting in vector") {
            ranges_type ranges{{"i", {0, 2}}, {"j", {1, 4}}, {"k", {2, 6}}};

            SECTION("i = i * jk") {
                IndexMap im("i", "i", "j, k");
                std::array a_data{2.0, 3.0};
                std::array b_data{4.0,  5.0,  6.0,  7.0,  8.0,  9.0,
                                  10.0, 11.0, 12.0, 13.0, 14.0, 15.0};
                std::array c_data{228.0, 342.0};

                auto&& [orange, lrange, rrange] = im.select(ranges);
                TA::TensorD a(make_range(lrange), a_data.data());
                TA::TensorD b(make_range(rrange), b_data.data());
                TA::TensorD corr(make_range(orange), c_data.data());

                auto result = block_kernel(im, ranges, a, b);
                REQUIRE((result - corr).abs_max() < 1E-6);
            }

            SECTION("i = j * ik") {
                IndexMap im("i", "j", "i, k");
                std::array a_data{2.0, 3.0, 4.0};
                std::array b_data{5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0};
                std::array c_data{234.0, 378.0};

                auto&& [orange, lrange, rrange] = im.select(ranges);
                TA::TensorD a(make_range(lrange), a_data.data());
                TA::TensorD b(make_range(rrange), b_data.data());
                TA::TensorD corr(make_range(orange), c_data.data());

                auto result = block_kernel(im, ranges, a, b);
                REQUIRE((result - corr).abs_max() < 1E-6);
            }

            SECTION("i = j * ki") {
                IndexMap im("i", "j", "k, i");
                std::array a_data{2.0, 3.0, 4.0};
                std::array b_data{5.0, 9.0, 6.0, 10.0, 7.0, 11.0, 8.0, 12.0};
                std::array c_data{234.0, 378.0};

                auto&& [orange, lrange, rrange] = im.select(ranges);
                TA::TensorD a(make_range(lrange), a_data.data());
                TA::TensorD b(make_range(rrange), b_data.data());
                TA::TensorD corr(make_range(orange), c_data.data());

                auto result = block_kernel(im, ranges, a, b);
                REQUIRE((result - corr).abs_max() < 1E-6);
            }
        }

        SECTION("Three indices resulting in matrix") {
            ranges_type ranges{{"i", {0, 2}}, {"j", {1, 4}}, {"k", {2, 6}}};

            SECTION("ij = i * jk") {
                IndexMap im("i,j", "i", "j, k");
                std::array a_data{2.0, 3.0};
                std::array b_data{4.0,  5.0,  6.0,  7.0,  8.0,  9.0,
                                  10.0, 11.0, 12.0, 13.0, 14.0, 15.0};
                std::array c_data{44.0, 76.0, 108.0, 66.0, 114.0, 162.0};

                auto&& [orange, lrange, rrange] = im.select(ranges);
                TA::TensorD a(make_range(lrange), a_data.data());
                TA::TensorD b(make_range(rrange), b_data.data());
                TA::TensorD corr(make_range(orange), c_data.data());

                auto result = block_kernel(im, ranges, a, b);
                REQUIRE((result - corr).abs_max() < 1E-6);
            }

            SECTION("ij = j * ik") {
                IndexMap im("i, j", "j", "i, k");
                std::array a_data{2.0, 3.0, 4.0};
                std::array b_data{5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0};
                std::array c_data{52.0, 78.0, 104.0, 84.0, 126.0, 168.0};

                auto&& [orange, lrange, rrange] = im.select(ranges);
                TA::TensorD a(make_range(lrange), a_data.data());
                TA::TensorD b(make_range(rrange), b_data.data());
                TA::TensorD corr(make_range(orange), c_data.data());

                auto result = block_kernel(im, ranges, a, b);
                REQUIRE((result - corr).abs_max() < 1E-6);
            }

            SECTION("ij = j * ki") {
                IndexMap im("i,j", "j", "k, i");
                std::array a_data{2.0, 3.0, 4.0};
                std::array b_data{5.0, 9.0, 6.0, 10.0, 7.0, 11.0, 8.0, 12.0};
                std::array c_data{52.0, 78.0, 104.0, 84.0, 126.0, 168.0};

                auto&& [orange, lrange, rrange] = im.select(ranges);
                TA::TensorD a(make_range(lrange), a_data.data());
                TA::TensorD b(make_range(rrange), b_data.data());
                TA::TensorD corr(make_range(orange), c_data.data());

                auto result = block_kernel(im, ranges, a, b);
                REQUIRE((result - corr).abs_max() < 1E-6);
            }

            SECTION("ik = i * jk") {
                IndexMap im("i,k", "i", "j, k");
                std::array a_data{2.0, 3.0};
                std::array b_data{4.0,  5.0,  6.0,  7.0,  8.0,  9.0,
                                  10.0, 11.0, 12.0, 13.0, 14.0, 15.0};
                std::array c_data{48.0, 54.0, 60.0, 66.0,
                                  72.0, 81.0, 90.0, 99.0};

                auto&& [orange, lrange, rrange] = im.select(ranges);
                TA::TensorD a(make_range(lrange), a_data.data());
                TA::TensorD b(make_range(rrange), b_data.data());
                TA::TensorD corr(make_range(orange), c_data.data());

                auto result = block_kernel(im, ranges, a, b);
                REQUIRE((result - corr).abs_max() < 1E-6);
            }

            SECTION("ik = j * ik") {
                IndexMap im("i, k", "j", "i, k");
                std::array a_data{2.0, 3.0, 4.0};
                std::array b_data{5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0};
                std::array c_data{45.0, 54.0, 63.0, 72.0,
                                  81.0, 90.0, 99.0, 108.0};

                auto&& [orange, lrange, rrange] = im.select(ranges);
                TA::TensorD a(make_range(lrange), a_data.data());
                TA::TensorD b(make_range(rrange), b_data.data());
                TA::TensorD corr(make_range(orange), c_data.data());

                auto result = block_kernel(im, ranges, a, b);
                REQUIRE((result - corr).abs_max() < 1E-6);
            }

            SECTION("ik = j * ki") {
                IndexMap im("i,k", "j", "k, i");
                std::array a_data{2.0, 3.0, 4.0};
                std::array b_data{5.0, 9.0, 6.0, 10.0, 7.0, 11.0, 8.0, 12.0};
                std::array c_data{45.0, 54.0, 63.0, 72.0,
                                  81.0, 90.0, 99.0, 108.0};

                auto&& [orange, lrange, rrange] = im.select(ranges);
                TA::TensorD a(make_range(lrange), a_data.data());
                TA::TensorD b(make_range(rrange), b_data.data());
                TA::TensorD corr(make_range(orange), c_data.data());

                auto result = block_kernel(im, ranges, a, b);
                REQUIRE((result - corr).abs_max() < 1E-6);
            }

            SECTION("jk = i * jk") {
                IndexMap im("j,k", "i", "j, k");
                std::array a_data{2.0, 3.0};
                std::array b_data{4.0,  5.0,  6.0,  7.0,  8.0,  9.0,
                                  10.0, 11.0, 12.0, 13.0, 14.0, 15.0};
                std::array c_data{20.0, 25.0, 30.0, 35.0, 40.0, 45.0,
                                  50.0, 55.0, 60.0, 65.0, 70.0, 75.0};

                auto&& [orange, lrange, rrange] = im.select(ranges);
                TA::TensorD a(make_range(lrange), a_data.data());
                TA::TensorD b(make_range(rrange), b_data.data());
                TA::TensorD corr(make_range(orange), c_data.data());

                auto result = block_kernel(im, ranges, a, b);
                REQUIRE((result - corr).abs_max() < 1E-6);
            }

            SECTION("jk = j * ik") {
                IndexMap im("j, k", "j", "i, k");
                std::array a_data{2.0, 3.0, 4.0};
                std::array b_data{5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0};
                std::array c_data{28.0, 32.0, 36.0, 40.0, 42.0, 48.0,
                                  54.0, 60.0, 56.0, 64.0, 72.0, 80.0};

                auto&& [orange, lrange, rrange] = im.select(ranges);
                TA::TensorD a(make_range(lrange), a_data.data());
                TA::TensorD b(make_range(rrange), b_data.data());
                TA::TensorD corr(make_range(orange), c_data.data());

                auto result = block_kernel(im, ranges, a, b);
                REQUIRE((result - corr).abs_max() < 1E-6);
            }

            SECTION("jk = j * ki") {
                IndexMap im("j,k", "j", "k, i");
                std::array a_data{2.0, 3.0, 4.0};
                std::array b_data{5.0, 9.0, 6.0, 10.0, 7.0, 11.0, 8.0, 12.0};
                std::array c_data{28.0, 32.0, 36.0, 40.0, 42.0, 48.0,
                                  54.0, 60.0, 56.0, 64.0, 72.0, 80.0};

                auto&& [orange, lrange, rrange] = im.select(ranges);
                TA::TensorD a(make_range(lrange), a_data.data());
                TA::TensorD b(make_range(rrange), b_data.data());
                TA::TensorD corr(make_range(orange), c_data.data());

                auto result = block_kernel(im, ranges, a, b);
                REQUIRE((result - corr).abs_max() < 1E-6);
            }
        }

        SECTION("Three indices resulting in rank 3 tensor") {
            ranges_type ranges{{"i", {0, 2}}, {"j", {1, 4}}, {"k", {2, 6}}};

            SECTION("ijk = i * jk") {
                IndexMap im("i,j, k", "i", "j, k");
                std::array a_data{2.0, 3.0};
                std::array b_data{4.0,  5.0,  6.0,  7.0,  8.0,  9.0,
                                  10.0, 11.0, 12.0, 13.0, 14.0, 15.0};
                std::array c_data{8.0,  10.0, 12.0, 14.0, 16.0, 18.0,
                                  20.0, 22.0, 24.0, 26.0, 28.0, 30.0,

                                  12.0, 15.0, 18.0, 21.0, 24.0, 27.0,
                                  30.0, 33.0, 36.0, 39.0, 42.0, 45.0};

                auto&& [orange, lrange, rrange] = im.select(ranges);
                TA::TensorD a(make_range(lrange), a_data.data());
                TA::TensorD b(make_range(rrange), b_data.data());
                TA::TensorD corr(make_range(orange), c_data.data());

                auto result = block_kernel(im, ranges, a, b);
                REQUIRE((result - corr).abs_max() < 1E-6);
            }

            SECTION("ijk = j * ik") {
                IndexMap im("i, j, k", "j", "i, k");
                std::array a_data{2.0, 3.0, 4.0};
                std::array b_data{5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0};
                std::array c_data{10.0, 12.0, 14.0, 16.0, 15.0, 18.0,
                                  21.0, 24.0, 20.0, 24.0, 28.0, 32.0,

                                  18.0, 20.0, 22.0, 24.0, 27.0, 30.0,
                                  33.0, 36.0, 36.0, 40.0, 44.0, 48.0};

                auto&& [orange, lrange, rrange] = im.select(ranges);
                TA::TensorD a(make_range(lrange), a_data.data());
                TA::TensorD b(make_range(rrange), b_data.data());
                TA::TensorD corr(make_range(orange), c_data.data());

                auto result = block_kernel(im, ranges, a, b);
                REQUIRE((result - corr).abs_max() < 1E-6);
            }

            SECTION("ijk = j * ki") {
                IndexMap im("i,j,k", "j", "k, i");
                std::array a_data{2.0, 3.0, 4.0};
                std::array b_data{5.0, 9.0, 6.0, 10.0, 7.0, 11.0, 8.0, 12.0};
                std::array c_data{10.0, 12.0, 14.0, 16.0, 15.0, 18.0,
                                  21.0, 24.0, 20.0, 24.0, 28.0, 32.0,

                                  18.0, 20.0, 22.0, 24.0, 27.0, 30.0,
                                  33.0, 36.0, 36.0, 40.0, 44.0, 48.0};

                auto&& [orange, lrange, rrange] = im.select(ranges);
                TA::TensorD a(make_range(lrange), a_data.data());
                TA::TensorD b(make_range(rrange), b_data.data());
                TA::TensorD corr(make_range(orange), c_data.data());

                auto result = block_kernel(im, ranges, a, b);
                REQUIRE((result - corr).abs_max() < 1E-6);
            }

            SECTION("jki = i * jk") {
                IndexMap im("j, k, i", "i", "j, k");
                std::array a_data{2.0, 3.0};
                std::array b_data{4.0,  5.0,  6.0,  7.0,  8.0,  9.0,
                                  10.0, 11.0, 12.0, 13.0, 14.0, 15.0};
                std::array c_data{
                  8.0,  12.0, 10.0, 15.0, 12.0, 18.0, 14.0, 21.0,

                  16.0, 24.0, 18.0, 27.0, 20.0, 30.0, 22.0, 33.0,

                  24.0, 36.0, 26.0, 39.0, 28.0, 42.0, 30.0, 45.0};

                auto&& [orange, lrange, rrange] = im.select(ranges);
                TA::TensorD a(make_range(lrange), a_data.data());
                TA::TensorD b(make_range(rrange), b_data.data());
                TA::TensorD corr(make_range(orange), c_data.data());

                auto result = block_kernel(im, ranges, a, b);
                REQUIRE((result - corr).abs_max() < 1E-6);
            }

            SECTION("jki = j * ik") {
                IndexMap im("j, k, i", "j", "i, k");
                std::array a_data{2.0, 3.0, 4.0};
                std::array b_data{5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0};
                std::array c_data{
                  10.0, 18.0, 12.0, 20.0, 14.0, 22.0, 16.0, 24.0,

                  15.0, 27.0, 18.0, 30.0, 21.0, 33.0, 24.0, 36.0,

                  20.0, 36.0, 24.0, 40.0, 28.0, 44.0, 32.0, 48.0};

                auto&& [orange, lrange, rrange] = im.select(ranges);
                TA::TensorD a(make_range(lrange), a_data.data());
                TA::TensorD b(make_range(rrange), b_data.data());
                TA::TensorD corr(make_range(orange), c_data.data());

                auto result = block_kernel(im, ranges, a, b);
                REQUIRE((result - corr).abs_max() < 1E-6);
            }

            SECTION("jki = j * ki") {
                IndexMap im("j,k,i", "j", "k, i");
                std::array a_data{2.0, 3.0, 4.0};
                std::array b_data{5.0, 9.0, 6.0, 10.0, 7.0, 11.0, 8.0, 12.0};
                std::array c_data{
                  10.0, 18.0, 12.0, 20.0, 14.0, 22.0, 16.0, 24.0,

                  15.0, 27.0, 18.0, 30.0, 21.0, 33.0, 24.0, 36.0,

                  20.0, 36.0, 24.0, 40.0, 28.0, 44.0, 32.0, 48.0};

                auto&& [orange, lrange, rrange] = im.select(ranges);
                TA::TensorD a(make_range(lrange), a_data.data());
                TA::TensorD b(make_range(rrange), b_data.data());
                TA::TensorD corr(make_range(orange), c_data.data());

                auto result = block_kernel(im, ranges, a, b);
                REQUIRE((result - corr).abs_max() < 1E-6);
            }

            SECTION("kij = i * jk") {
                IndexMap im("k, i, j", "i", "j, k");
                std::array a_data{2.0, 3.0};
                std::array b_data{4.0,  5.0,  6.0,  7.0,  8.0,  9.0,
                                  10.0, 11.0, 12.0, 13.0, 14.0, 15.0};
                std::array c_data{8.0,  16.0, 24.0, 12.0, 24.0, 36.0,

                                  10.0, 18.0, 26.0, 15.0, 27.0, 39.0,

                                  12.0, 20.0, 28.0, 18.0, 30.0, 42.0,

                                  14.0, 22.0, 30.0, 21.0, 33.0, 45.0};

                auto&& [orange, lrange, rrange] = im.select(ranges);
                TA::TensorD a(make_range(lrange), a_data.data());
                TA::TensorD b(make_range(rrange), b_data.data());
                TA::TensorD corr(make_range(orange), c_data.data());

                auto result = block_kernel(im, ranges, a, b);
                REQUIRE((result - corr).abs_max() < 1E-6);
            }

            SECTION("kij = j * ik") {
                IndexMap im("k, i, j", "j", "i, k");
                std::array a_data{2.0, 3.0, 4.0};
                std::array b_data{5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0};
                std::array c_data{10.0, 15.0, 20.0, 18.0, 27.0, 36.0,

                                  12.0, 18.0, 24.0, 20.0, 30.0, 40.0,

                                  14.0, 21.0, 28.0, 22.0, 33.0, 44.0,

                                  16.0, 24.0, 32.0, 24.0, 36.0, 48.0};

                auto&& [orange, lrange, rrange] = im.select(ranges);
                TA::TensorD a(make_range(lrange), a_data.data());
                TA::TensorD b(make_range(rrange), b_data.data());
                TA::TensorD corr(make_range(orange), c_data.data());

                auto result = block_kernel(im, ranges, a, b);
                REQUIRE((result - corr).abs_max() < 1E-6);
            }

            SECTION("kij = j * ki") {
                IndexMap im("k, i,j", "j", "k, i");
                std::array a_data{2.0, 3.0, 4.0};
                std::array b_data{5.0, 9.0, 6.0, 10.0, 7.0, 11.0, 8.0, 12.0};
                std::array c_data{10.0, 15.0, 20.0, 18.0, 27.0, 36.0,

                                  12.0, 18.0, 24.0, 20.0, 30.0, 40.0,

                                  14.0, 21.0, 28.0, 22.0, 33.0, 44.0,

                                  16.0, 24.0, 32.0, 24.0, 36.0, 48.0};

                auto&& [orange, lrange, rrange] = im.select(ranges);
                TA::TensorD a(make_range(lrange), a_data.data());
                TA::TensorD b(make_range(rrange), b_data.data());
                TA::TensorD corr(make_range(orange), c_data.data());

                auto result = block_kernel(im, ranges, a, b);
                REQUIRE((result - corr).abs_max() < 1E-6);
            }
        }
    }
}