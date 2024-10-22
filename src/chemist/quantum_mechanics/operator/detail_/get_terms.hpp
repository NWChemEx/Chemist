#pragma once
#include <chemist/quantum_mechanics/operator/operator_visitor.hpp>

namespace chemist::qm_operator::detail_ {

class EHVisitor : public OperatorVisitor {
public:
    EHVisitor() : OperatorVisitor(false) {}
    void run(const Kinetic<ManyElectrons>&) { m_is_good_term = true; }
    void run(const Coulomb<ManyElectrons, Nuclei>&) { m_is_good_term = true; }
    void run(const Coulomb<ManyElectrons, ManyElectrons>&) {
        m_is_good_term = true;
    }
    bool m_is_good_term = false;
};

class CHVisitor : public OperatorVisitor {
public:
    CHVisitor() : OperatorVisitor(false) {}
    void run(const Kinetic<ManyElectrons>&) { m_is_good_term = true; }
    void run(const Coulomb<ManyElectrons, Nuclei>&) { m_is_good_term = true; }
    bool m_is_good_term = false;
};

template<typename ReturnType, typename VisitorType, typename ContainerType>
ReturnType get_terms(ContainerType&& container) {
    using clean_type     = std::decay_t<ContainerType>;
    using container_type = typename clean_type::container_type;
    container_type ops;
    auto size = container.size();
    for(decltype(size) i = 0; i < size; ++i) {
        const auto& op = container.get_operator(i);
        EHVisitor visitor;
        op.visit(visitor);
        if(visitor.m_is_good_term) {
            ops.emplace_back(container.coefficient(i), op.clone());
        }
    }
    return ReturnType(std::move(ops));
}

} // namespace chemist::qm_operator::detail_