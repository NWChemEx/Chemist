/*
 * Copyright 2025 NWChemEx-Project
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

#pragma once
#include <wtf/wtf.hpp>

/** @brief Contains concepts used throughout Chemist.
 *
 *  Chemist does not presently define any concepts of its own; instead this
 *  namespace re-exports concepts from Chemist's dependencies (e.g.,
 *  WeaklyTypedFloat) so that downstream code can spell them as
 *  `chemist::concepts::...` without needing to know which dependency
 *  actually defines them.
 */
namespace chemist::concepts {

/// Imported from wtf::concepts::FloatingPoint. See that concept's
/// documentation for more details.
using wtf::concepts::FloatingPoint;

} // namespace chemist::concepts
