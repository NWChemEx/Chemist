#pragma once

/** @file operator_fwd.hpp
 *
 *  This file contains forward declarations of the operator types found in
 *  Chemist.
 */

namespace chemist::qm_operator {
template<typename T>
class Kinetic;

template<typename T, typename U>
class Coulomb;

template<typename T, typename U>
class Exchange;

template<typename T, typename U>
class ExchangeCorrelation;

} // namespace chemist::qm_operator