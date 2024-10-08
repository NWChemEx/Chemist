/*
 * Copyright 2023 NWChemEx-Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "../catch.hpp"
#include <chemist/basis_set/atomic_basis_set.hpp>
#include <chemist/basis_set/atomic_basis_set_traits.hpp>
#include <chemist/basis_set/atomic_basis_set_view.hpp>

using namespace chemist::basis_set;

template<typename T>
void test_construction() {
    T instance;
}

TEMPLATE_TEST_CASE("AtomicBasisSetTraits", "", float, double) {
    using abs_type =
      AtomicBasisSet<Shell<ContractedGaussian<Primitive<TestType>>>>;

    REQUIRE_NOTHROW(AtomicBasisSetTraits<abs_type>());

    REQUIRE_NOTHROW(AtomicBasisSetTraits<AtomicBasisSetView<abs_type>>());

    REQUIRE_NOTHROW(AtomicBasisSetTraits<AtomicBasisSetView<const abs_type>>());
}
