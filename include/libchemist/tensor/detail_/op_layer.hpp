#pragma once

namespace libchemist::tensor::detail_ {

template<typename DerivedType>
class OpLayer {
public:
    const DerivedType& downcast() const noexcept;
    DerivedType& downcast() noexcept;
};

template<typename DerivedType>
const DerivedType& OpLayer<DerivedType>::downcast() const noexcept {
    return static_cast<const DerivedType&>(*this);
}

template<typename DerivedType>
DerivedType& OpLayer<DerivedType>::downcast() noexcept {
    return static_cast<DerivedType&>(*this);
}

} // namespace libchemist::tensor::detail_
