#pragma once
#include <array>

namespace libchemist::type {

/// The type of a natural number (including 0)
using size = std::size_t;

/// The type of the structure holding a Cartesian coordinate
template<typename T>
using coord = std::array<T, 3>;

/// Type used to hold the magnitude of the angular momentum
using angular_momentum = unsigned short;

} // namespace libchemist::type
