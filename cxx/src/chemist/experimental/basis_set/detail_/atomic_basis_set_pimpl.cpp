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

#include <chemist/experimental/basis_set/detail_/atomic_basis_set_pimpl.hpp>

namespace chemist::experimental::detail_ {

template class AtomicBasisSetPIMPL<CCAShell<CartesianAO>>;
template class AtomicBasisSetPIMPL<CCAShell<SphericalAO>>;
template class AtomicBasisSetViewPIMPL<CCAShell<CartesianAO>>;
template class AtomicBasisSetViewPIMPL<CCAShell<SphericalAO>>;
template class AtomicBasisSetViewPIMPL<const CCAShell<CartesianAO>>;
template class AtomicBasisSetViewPIMPL<const CCAShell<SphericalAO>>;

} // namespace chemist::experimental::detail_
