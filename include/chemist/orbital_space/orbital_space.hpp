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
