/*
 * Copyright 2024 NWChemEx-Project
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

#include "../test_helpers.hpp"
#include <map>
#include <tuple>
#include <vector>

/* Testing strategy.
 *
 * The DSL component doesn't really care what the types are as long as they
 * are comparable via operator==. So we test with built-in types:
 *
 * - double
 * - int
 *
 * and "user-defined" (actually defined in standard library) types like:
 *
 * - std::vector<int>
 * - std::map<double, int>.
 *
 * For the purposes of integration testing, the tests in operator_impl.cpp
 * will ensure that operators can be added together.
 */

namespace test_chemist {

/// Provides a tuple of values to initialize unary DSL terms with
inline auto unary_values() {
    return std::make_tuple(double{3.14}, int{42}, std::vector<int>{1, 2, 3},
                           std::map<double, int>{{3.14, 42}});
}

/// Tuple type returned from unary_values
using unary_types = decltype(unary_values());

/// Provides a tuple of pairs of values to initialize binary DSL terms with
inline auto binary_values() {
    auto [v0, v1, v2, v3] = unary_values();
    return std::make_tuple(std::pair(v0, v0), std::pair(v0, v1),
                           std::pair(v2, v3), std::pair(v3, v3));
}

/// Tuple type returned from binary_values
using binary_types = decltype(binary_values());

} // namespace test_chemist
