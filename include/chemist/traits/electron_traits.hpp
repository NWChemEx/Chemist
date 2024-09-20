#pragma once
#include <chemist/traits/chemist_class_traits.hpp>

namespace chemist {

class Electron;
class ManyElectrons;

template<>
struct ChemistClassTraits<Electron> {
    using value_type      = Electron;
    using reference       = value_type;
    using const_reference = value_type;
    using view_type       = value_type;
    using const_view_type = value_type;
};

template<>
struct ChemistClassTraits<const Electron> {
    using value_type      = Electron;
    using reference       = value_type;
    using const_reference = value_type;
    using view_type       = value_type;
    using const_view_type = value_type;
};

template<>
struct ChemistClassTraits<ManyElectrons> {
    using value_type      = ManyElectrons;
    using reference       = value_type&;
    using const_reference = const value_type&;
    using view_type       = value_type&;
    using const_view_type = const value_type&;
};

template<>
struct ChemistClassTraits<const ManyElectrons> {
    using value_type      = ManyElectrons;
    using reference       = value_type&;
    using const_reference = const value_type&;
    using view_type       = value_type&;
    using const_view_type = const value_type&;
};

} // namespace chemist