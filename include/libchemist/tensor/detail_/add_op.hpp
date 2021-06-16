#pragma once

namespace libchemist::tensor::detail_ {

template<typename LHSType, typename RHSType>
class AddOp : public OpLayer<AddOp<LHSType, RHSType>> {
public:
    template<typename ResultType>
    evaluate(ResultType&& r)
private:
    LHSType& m_lhs_;
    RHSType& m_rhs_;
};

template<typename LHSType, typename RHSType>
template<typename ResultType>
auto evaluate(ResultType&& r) {
    auto lhs = m_lhs_.evaluate(r);
    auto rhs = m_rhs_.evaluate(r);

    
}

}
