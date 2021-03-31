#pragma once
#include "libchemist/basis_set/basis_set.hpp"
#include "libchemist/orbital_space/ao_space.hpp"
#include "libchemist/orbital_space/base_space.hpp"
#include "libchemist/orbital_space/canonical_space.hpp"
#include "libchemist/orbital_space/dependent_space.hpp"
#include "libchemist/orbital_space/derived_space.hpp"
#include "libchemist/ta_helpers/ta_hashers.hpp"

namespace libchemist::orbital_space {

template<typename T>
using AOSpace = AOSpace_<AOBasisSet<T>, BaseSpace<T>>;

template<typename T>
using SparseAOSpace = AOSpace_<AOBasisSet<T>, SparseDependentBase<T>>;

template<typename T>
using DerivedSpace = DerivedSpace_<type::tensor<T>, AOSpace<T>, BaseSpace<T>>;

/** @brief Type of the independent space driving a sparse computation.
 *
 *  In many of the sparse methods dependent indices are constrained by the
 *  localized molecular orbitals (or tuples of them). The LMOs are the
 *  independent space in this scenario. Generally speaking the transformation
 *  from AOs to the LMOs will be sparse. The sparse map for this is stored in
 *  the SparseAOSpace "from space". Since the LMOs (or tuples of them) span the
 *  independent space there is no sparse map from them to the independent space
 *  (well there is, but it's trivial i->i mapping) so we define the base space
 *  for the SparseIndependentSpace to be BaseSpace instead of SparseBase.
 */
template<typename T>
using SparseIndependentSpace =
  DerivedSpace_<type::tensor_of_tensors<T>, SparseAOSpace<T>, BaseSpace<T>>;

template<typename T>
using SparseDerivedSpace =
  DerivedSpace_<type::tensor_of_tensors<T>, SparseAOSpace<T>, SparseBase<T>>;

template<typename T>
using CanonicalSpace = CanonicalSpace_<type::tensor<T>, DerivedSpace<T>>;

template<typename T>
using SparseCanonicalSpace =
  CanonicalSpace_<type::tensor_of_tensors<T>, SparseDerivedSpace<T>>;

template<typename T>
using SparseIndependentCanonicalSpace =
  CanonicalSpace_<type::tensor_of_tensors<T>, SparseIndependentSpace<T>>;

// Instantiate some common specializations
extern template class BaseSpace_<type::tensor<double>>;
extern template class BaseSpace_<type::tensor<float>>;
extern template class BaseSpace_<type::tensor_of_tensors<double>>;
extern template class BaseSpace_<type::tensor_of_tensors<float>>;

extern template class DependentBaseSpace_<sparse_map::SparseMapEE,
                                          type::tensor_of_tensors<double>>;
extern template class DependentBaseSpace_<sparse_map::SparseMapEE,
                                          type::tensor_of_tensors<float>>;

extern template class AOSpace_<AOBasisSet<double>, BaseSpace<double>>;
extern template class AOSpace_<AOBasisSet<float>, BaseSpace<float>>;
extern template class AOSpace_<AOBasisSet<double>, SparseDependentBase<double>>;
extern template class AOSpace_<AOBasisSet<float>, SparseDependentBase<float>>;

} // namespace libchemist::orbital_space
