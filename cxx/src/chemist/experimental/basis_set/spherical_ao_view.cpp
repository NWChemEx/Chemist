/*
 * Copyright 2026 NWChemEx-Project
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

#include <chemist/experimental/basis_set/spherical_ao_view.hpp>

namespace chemist::experimental {

// Unlike CartesianAOView, it is the read-only instantiation which is listed
// here, since it is the only one SphericalAOView supports. Its assignment
// operators are deleted rather than static_assert-ed, so instantiating it
// explicitly is safe.
template class SphericalAOView<const SphericalAO>;

} // namespace chemist::experimental
