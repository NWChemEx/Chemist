#pragma once
#include <chemist/chemical_system/electron/electron.hpp>
#include <chemist/quantum_mechanics/operator/kinetic.hpp>

namespace chemist::qm_operator {

template<typename T>
class Kinetic : public OperatorBase {
private:
    using base_type = OperatorBase;

public:
    using base_type::visitor_reference;

private:
    void visit_(visitor_reference visitor) const override;
};

extern template class Kinetic<Electron>;

} // namespace chemist::qm_operator