#include "chemist_examples.hpp"
#include <catch2/catch.hpp>

using namespace chemist_examples;
TEST_CASE("Doc snippets") { REQUIRE(nucleus_example() == 0); }
