/*
 * Copyright 2022 NWChemEx-Project
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

/** @file orbital_space.hpp
 *
 *  @brief Convenience header for including the orbital space component of
 *         LibChemist.
 *
 *  This header file will bring into scope all of the classes associated with
 *  orbital spaces. This header is intended for downstream usage only and should
 *  in general not be included in any other LibChemist file.
 */

#include "chemist/orbital_space/ao_space.hpp"
#include "chemist/orbital_space/base_space.hpp"
#include "chemist/orbital_space/canonical_space.hpp"
#include "chemist/orbital_space/derived_space.hpp"
#include "chemist/orbital_space/independent_space.hpp"
#include "chemist/orbital_space/type_traits/type_traits.hpp"
