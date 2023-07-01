// /*
//  * Copyright 2022 NWChemEx-Project
//  *
//  * Licensed under the Apache License, Version 2.0 (the "License");
//  * you may not use this file except in compliance with the License.
//  * You may obtain a copy of the License at
//  *
//  * http://www.apache.org/licenses/LICENSE-2.0
//  *
//  * Unless required by applicable law or agreed to in writing, software
//  * distributed under the License is distributed on an "AS IS" BASIS,
//  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  * See the License for the specific language governing permissions and
//  * limitations under the License.
//  */

// #include "chemist/basis_set/ao_basis_set/ao_basis_set_pimpl.hpp"
// #include <catch2/catch.hpp>
// #include <utility>

// using pimpl_t  = chemist::detail_::AOBasisSetPIMPL<double>;
// using center_t = chemist::AtomicBasisSet<double>;

// TEST_CASE("AOBasisSetPIMPL : default ctor") {
//     pimpl_t p{};
//     REQUIRE(p.size() == 0);
// }

// TEST_CASE("AOBasisSetPIMPL : size") {
//     pimpl_t p;
//     p.add_center(center_t("", 0, 1.0, 2.0, 3.0));
//     REQUIRE(p.size() == 1);
// }

// TEST_CASE("AOBasisSetPIMPL : add_center") {
//     pimpl_t p;
//     p.add_center(center_t("", 0, 1.0, 2.0, 3.0));
//     REQUIRE(p.at(0) == center_t("", 0, 1.0, 2.0, 3.0));
// }

// TEST_CASE("AOBasisSetPIMPL : at") {
//     pimpl_t p;
//     p.add_center(center_t("", 0, 1.0, 2.0, 3.0));
//     REQUIRE(p.at(0) == center_t("", 0, 1.0, 2.0, 3.0));
// }

// TEST_CASE("AOBasisSetPIMPL : at() const") {
//     pimpl_t p;
//     p.add_center(center_t("", 0, 1.0, 2.0, 3.0));
//     REQUIRE(std::as_const(p).at(0) == center_t("", 0, 1.0, 2.0, 3.0));
// }
