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

#pragma once

namespace chemist {

/** @brief Represents a single electron.
 *
 *  The Electron class is essentially a strong type used to create other types.
 *  Each object has no state and some minimal features.
 */
class Electron {
public:
    /** @brief Determines if *this is value equal to another object.
     *
     *  At present all Electron objects are value equal. This is because
     *  electrons are indistinguishable particles. Electron has no members,
     *  so the defaulted comparison (always true) already gives exactly this
     *  behavior.
     *
     *  Declared as a hidden friend rather than a member: GCC (at least
     *  through 10.2.1, the compiler manylinux2014 ships) has a bug where a
     *  same-type *member* operator== -- defaulted or hand-written -- is
     *  treated as two equally-viable C++20 rewritten-candidate overloads
     *  (the direct call and its reversed-argument synthesis) for the exact
     *  same signature, so `a == b` fails with "request for member
     *  'operator==' is ambiguous". A hidden-friend (non-member) defaulted
     *  operator== isn't affected. operator!= is synthesized automatically
     *  from this under C++20 rewritten-candidate rules, so it needs no
     *  separate declaration.
     */
    friend bool operator==(const Electron&, const Electron&) noexcept = default;
};

} // namespace chemist
