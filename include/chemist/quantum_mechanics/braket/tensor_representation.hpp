#pragma once
#include <chemist/quantum_mechanics/braket/detail_/braket_base.hpp>
#include <tensorwrapper/tensorwrapper.hpp>

namespace chemist::braket {

/** @brief Denotes that a BraKet is a tensor representation of an operator.
 *
 *  This class is a stub for the moment and will be fleshed out later.
 */
class TensorRepresentation : public detail_::BraKetBase<TensorRepresentation> {
private:
    /// Type of *this
    using my_type = TensorRepresentation;

    /// Type *this actually derives from
    using bra_ket_base_type = BraKetBase<my_type>;

public:
    /// Type *this evaluates to
    using result_type = tensorwrapper::Tensor;
};

} // namespace chemist::braket