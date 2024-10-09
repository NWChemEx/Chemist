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

/** @file dsl_fwd.hpp
 *
 *  This file forward declares the classes needed to power the DSL. The
 *  forward declarations are primarily useful for declaring interfaces and
 *  for template meta-programming.
 */

namespace chemist::dsl {

template<typename LHSType, typename RHSType>
class Add;

template<typename DerivedType, typename LHSType, typename RHSType>
class BinaryOp;

template<typename LHSType, typename RHSType>
class Multiply;

template<typename DerivedType>
class Term;

template<typename T>
class TermTraits;

} // namespace chemist::dsl