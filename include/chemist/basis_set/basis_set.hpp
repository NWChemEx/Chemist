#pragma once
#include "chemist/basis_set/ao_basis_set.hpp"
#include "chemist/basis_set/atomic_basis_set.hpp"
#include "chemist/basis_set/contracted_gaussian.hpp"
#include "chemist/basis_set/contracted_gaussian_view.hpp"
#include "chemist/basis_set/primitive.hpp"
#include "chemist/basis_set/primitive_view.hpp"
#include "chemist/basis_set/shell.hpp"
#include "chemist/basis_set/shell_view.hpp"

namespace chemist {

using AOBasisSetD = AOBasisSet<double>;
using AOBasisSetF = AOBasisSet<float>;

} // namespace chemist