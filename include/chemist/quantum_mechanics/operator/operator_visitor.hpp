#pragma once
#include <chemist/chemical_system/electron/electron.hpp>
#include <chemist/chemical_system/nucleus/nucleus.hpp>

namespace chemist::qm_operator {
template<typename T>
class Kinetic;

#define OVERLOAD(T) virtual void run(T&)
#define CONST_OVERLOAD(T) virtual void run(const T&);
#define OVERLOADS(T) \
    OVERLOAD(T);     \
    CONST_OVERLOAD(T)

class OperatorVisitor {
public:
    OVERLOADS(Kinetic<Electron>);
    OVERLOADS(Kinetic<ManyElectrons>);
};

#undef OVERLOADS
#undef CONST_OVERLOAD
#undef OVERLOAD
} // namespace chemist::qm_operator