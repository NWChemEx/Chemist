#pragma once
#include <chemist/traits/chemist_class_traits.hpp>

namespace chemist {

template<typename... ParticleTypes>
class Density;

template<typename... ParticleTypes>
class DecomposableDensity;

template<typename... ParticleTypes>
struct ChemistClassTraits<Density<ParticleTypes...>> {
    using value_type      = Density<ParticleTypes...>;
    using reference       = value_type&;
    using const_reference = const value_type&;
    using view_type       = value_type;
    using const_view_type = value_type;
};

template<typename... ParticleTypes>
struct ChemistClassTraits<const Density<ParticleTypes...>> {
    using value_type      = Density<ParticleTypes...>;
    using reference       = const value_type&;
    using const_reference = const value_type&;
    using view_type       = value_type;
    using const_view_type = value_type;
};

template<typename... ParticleTypes>
struct ChemistClassTraits<DecomposableDensity<ParticleTypes...>> {
    using value_type      = DecomposableDensity<ParticleTypes...>;
    using reference       = value_type&;
    using const_reference = const value_type&;
    using view_type       = value_type;
    using const_view_type = value_type;
};

template<typename... ParticleTypes>
struct ChemistClassTraits<const DecomposableDensity<ParticleTypes...>> {
    using value_type      = DecomposableDensity<ParticleTypes...>;
    using reference       = const value_type&;
    using const_reference = const value_type&;
    using view_type       = value_type;
    using const_view_type = value_type;
};

} // namespace chemist