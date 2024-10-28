#pragma once
#include <chemist/quantum_mechanics/operator/operator_visitor.hpp>

namespace chemist::qm_operator::detail_ {

/// Determines which terms are in the Electronic Hamiltonian
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

/// Determines which terms are in the Core Hamiltonian
class CHVisitor : public OperatorVisitor {
public:
    CHVisitor() : OperatorVisitor(false) {}
    void run(const Kinetic<ManyElectrons>&) { m_is_good_term = true; }
    void run(const Coulomb<ManyElectrons, Nuclei>&) { m_is_good_term = true; }
    bool m_is_good_term = false;
};

/** @brief Wraps the process of selecting a subset of operator terms.
 *
 *  @tparam ReturnType The type of the operator containing the subset. The
 *                     caller must provide a value for @p ReturnType.
 *  @tparam VisitorType The type of the visitor that knows how to select the
 *                      subset. The caller must provide a value for
 *                      @p VisitorType.
 *  @tparam OperatorType The type of the object we are extracting the subset
 *                       from. Assumed to be derived from OperatorBase. The
 *                       value of this parameter will be deduced by the compiler
 *                       and the user does NOT need to provide it.
 *
 *  @param[in] container The operator which acts like a container of operators.
 *
 *  @return A newly created @p ReturnType object containing the subset of terms
 *          selected by a visitor of type @p VisitorType.
 *
 *  @throw std::bad_alloc if allocating the return fails. Strong throw
 *                        guarantee.
 */
template<typename ReturnType, typename VisitorType, typename OperatorType>
ReturnType get_terms(OperatorType&& container) {
    using clean_type     = std::decay_t<OperatorType>;
    using value_type     = typename clean_type::value_type;
    using container_type = std::vector<value_type>;
    container_type ops;
    auto size = container.size();
    for(decltype(size) i = 0; i < size; ++i) {
        const auto& op = container.get_operator(i);
        VisitorType visitor;
        op.visit(visitor);
        if(visitor.m_is_good_term) {
            ops.emplace_back(container.coefficient(i), op.clone());
        }
    }
    return ReturnType(std::move(ops));
}

} // namespace chemist::qm_operator::detail_