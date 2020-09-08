#pragma once

namespace libchemist::orbital_space {

template<typename T>
struct SparseTraits {
    using basis_set_type      = AOBasisSet<T>;
    using orbital_energy_type = type::tensor_of_tensors<T>;
    using overlap_type        = type::tensor_of_tensors<T>;
    using transform_type      = type::tensor_of_tensors<T>;
};

template<typename T>
struct DenseTraits {
    using basis_set_type      = AOBasisSet<T>;
    using orbital_energy_type = type::tensor<T>;
    using overlap_type        = type::tensor<T>;
    using transform_type      = type::tensor<T>;
};
template<typename T> using BaseSpace = BaseSpace_<DenseTraits<T>>;
template<typename T> using SparseBase = BaseSpace_<SparseTraits<T>>;
template<typename T> using AOSpace       = AOSpace_<DenseTraits<T>>;
template<typename T> using SparseAOSpace = AOSpace_<SparseTraits<T>>;
template<typename T> using DependentAOSpace = DependentSpace<SparseAOSpace<T>>;
template<typename T>
using DerivedSpace = DerivedSpace_<DenseTraits<T>>;
template<typename T>
using SparseDerivedSpace = DerivedSpace_<SparseTraits<T>, DependentAOSpace<T>>;
template<typename T> using Canonical = CanonicalSpace_<DenseTraits<T>>;
template<typename T> using SparseCanonical = CanonicalSpace_<SparseTraits<T>>;





} // namespace libchemist::orbital_space::detail_
