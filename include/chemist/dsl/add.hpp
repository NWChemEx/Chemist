#pragma once
#include <chemist/dsl/binary_op.hpp>
#include <type_traits>

namespace chemist::dsl {

template<typename LHSType, typename RHSType>
class Add : public BinaryOp<Add<LHSType, RHSType>, LHSType, RHSType> {
private:
    using my_type = Add<LHSType, RHSType>;
    using op_type = BinaryOp<my_type, LHSType, RHSType>;

public:
    using op_type::op_type;
};

} // namespace chemist::dsl