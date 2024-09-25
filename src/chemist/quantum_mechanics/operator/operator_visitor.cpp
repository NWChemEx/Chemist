#include <chemist/quantum_mechanics/operator/operator.hpp>

namespace chemist::qm_operator {

#define OVERLOAD(T)                                 \
    void OperatorVisitor::run(T& visitor) {         \
        return run(static_cast<const T&>(visitor)); \
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

#define ONE_PARTICLE_OVERLOADS(T) \
    OVERLOADS(T<Electron>);       \
    OVERLOADS(T<ManyElectrons>);  \
    OVERLOADS(T<Nucleus>);        \
    OVERLOADS(T<Nuclei>)

ONE_PARTICLE_OVERLOADS(Kinetic);

#undef ONE_PARTICLE_OVERLOADS
#undef OVERLOADS
#undef CONST_OVERLOAD
#undef OVERLOAD

} // namespace chemist::qm_operator