#include <chemist/quantum_mechanics/operator/operator.hpp>

namespace chemist::qm_operator {

#define OVERLOAD(T)                                                    \
    void OperatorVisitor::run(T&) {                                    \
        throw std::runtime_error(                                      \
          std::string("Visitor does not support operators of type ") + \
          std::string(#T));                                            \
    }

#define CONST_OVERLOAD(T)                                                    \
    void OperatorVisitor::run(const T&) {                                    \
        throw std::runtime_error(                                            \
          std::string("Visitor does not support operators of type const ") + \
          std::string(#T));                                                  \
    }

#define OVERLOADS(T) \
    OVERLOAD(T);     \
    CONST_OVERLOAD(T)

OVERLOADS(Kinetic<Electron>);
OVERLOADS(Kinetic<ManyElectrons>);

#undef OVERLOADS
#undef CONST_OVERLOAD
#undef OVERLOAD

} // namespace chemist::qm_operator