#pragma once
#include <chemist/traits/chemist_class_traits.hpp>
#include <chemist/traits/point_charge_traits.hpp>

namespace chemist {
class Nucleus;
template<typename T>
class NucleusView;
class Nuclei;
template<typename T>
class NucleiView;

template<>
struct ChemistClassTraits<Nucleus> {
    using value_type          = Nucleus;
    using reference           = NucleusView<value_type>;
    using const_reference     = NucleusView<const value_type>;
    using point_charge_traits = ChemistClassTraits<PointCharge<double>>;
};

template<>
struct ChemistClassTraits<const Nucleus> {
    using value_type          = Nucleus;
    using reference           = NucleusView<const value_type>;
    using const_reference     = NucleusView<const value_type>;
    using point_charge_traits = ChemistClassTraits<const PointCharge<double>>;
};

} // namespace chemist
