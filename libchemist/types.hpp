#pragma once
#include <tiledarray.h>

/** @file types.hpp
 *
 * Typedefs of fundamental types used in libchemist, just the tensor
 * class currently. 
 */
namespace libchemist {
namespace type {

/// Typedef of the tensor class
template<typename T>
using tensor = TA::TSpArray<T>;

} // namespace type
} // namespace libchemist
