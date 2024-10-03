#pragma once
#include <chemist/quantum_mechanics/braket/detail_/braket_base.hpp>

namespace chemist::braket {

/** @brief Base class for all bra-kets that are scalars.
 *
 *  This class is a stub for the moment.
 *
 *  @tparam ElementType The type of object that the BraKet evaluates to. The
 *                      exact type will depend on the operator, but ElementType
 *                      is usually a floating-point type.
 */
template<typename ElementType>
class TensorElement : public detail_::BraKetBase<TensorElement<ElementType>> {
public:
    /// Type of the BraKet's value
    using result_type = ElementType;
};

} // namespace chemist::braket