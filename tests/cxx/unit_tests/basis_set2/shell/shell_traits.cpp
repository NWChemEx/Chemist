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

#include <catch2/catch.hpp>
#include <chemist/basis_set2/shell/shell.hpp>
#include <chemist/basis_set2/shell/shell_traits.hpp>
#include <chemist/basis_set2/shell/shell_view.hpp>

using namespace chemist::basis_set;

TEMPLATE_TEST_CASE("ShellTraits", "", float, double) {
    using shell_type = Shell<ContractedGaussian<Primitive<TestType>>>;

    ShellTraits<shell_type> shell_traits;

    ShellTraits<ShellView<shell_type>> shell_view_traits;

    ShellTraits<ShellView<const shell_type>> const_shell_traits;
}
