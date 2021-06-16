#pragma once
#include "libchemist/tensor/detail_/op_fwd.hpp"

namespace libchemist::tensor::detail_ {

template<typename DerivedType>
class OpLayer {
public:
    template<typename RHSType>
    AddOp<DerivedType, RHSType> operator+(RHSType&& rhs) const;

    // template<typename RHSType>
    // SubtOp<DerivedType, RHSType> operator-(RHSType&& rhs) const;

    // template<typename RHSType>
    // MultOp<DerivedType, RHSType> operator*(RHSType&& rhs) const;
private:
    const DerivedType& downcast() const;
    DerivedType& downcast();
};

template<typename DerivedType>
template<typename RHSType>
AddOp<DerivedType, RHSType> OpLayer<DerivedType>::operator+(RHSType&& rhs) const{
    return AddOp<DerivedType, RHSType>(downcast(), std::forward<RHSType>(rhs));
}

// template<typename DerivedType>
// template<typename RHSType>
// SubtOp<DerivedType, RHSType> OpLayer<DerivedType>::operator-(RHSType&& rhs) const{
//     return SubtOp<DerivedType, RHSType>(downcast(), std::forward<RHSType>(rhs));
// }

// template<typename DerivedType>
// template<typename RHSType>
// MultOp<DerivedType, RHSType> OpLayer<DerivedType>::operator*(RHSType&& rhs) const{
//     return MultOp<DerivedType, RHSType>(downcast(), std::forward<RHSType>(rhs));
// }

}
