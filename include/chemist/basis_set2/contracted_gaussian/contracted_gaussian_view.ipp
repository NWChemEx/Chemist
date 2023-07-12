#pragma once

// This file is meant only for inclusion contracted_gaussian_view.hpp

namespace chemist::basis_set {

template<typename CGType>
template<typename CGType2, typename>
ContractedGaussianView<CGType>& ContractedGaussianView<CGType>::operator=(
  const CGType2& rhs) {
    assert_non_null_();
    if(rhs.is_null())
        throw std::runtime_error(
          "Assigning a null ContractedGaussian to a non-null "
          "ContractedGaussianView is currently not supported");
    if(rhs.size() != this->size())
        throw std::runtime_error("ContractedGaussian to assign must be the "
                                 "same size as the aliased one");
    center().x() = rhs.center().x();
    center().y() = rhs.center().y();
    center().z() = rhs.center().z();

    for(size_type prim_i = 0; prim_i < this->size(); ++prim_i) {
        (*this)[prim_i].coefficient() = rhs[prim_i].coefficient();
        (*this)[prim_i].exponent()    = rhs[prim_i].exponent();
    }
    return *this;
} // namespace >::operator=(constCGType2&rhs)

template<typename CGType>
template<typename OtherType, typename>
ContractedGaussianView<CGType>::ContractedGaussianView(const OtherType& other) :
  ContractedGaussianView(
    !other.is_null() ?
      ContractedGaussianView(other.size(), other[0].coefficient(),
                             other[0].exponent(), other.center()) :
      ContractedGaussianView()) {}

} // namespace chemist::basis_set
