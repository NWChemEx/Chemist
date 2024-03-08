#pragma once

namespace chemist {
class Nucleus;
class Nuclei;

template<typename NucleusType>
class NucleusView;

template<typename NucleiType>
class NucleiView;

template<typename ChemicalSystemClass>
struct ChemicalSystemClassTraits;

#define CS_TRAITS(CS_CLASS)                                     \
    template<>                                                  \
    struct ChemicalSystemClassTraits<CS_CLASS> {                \
        using value_type      = CS_CLASS;                       \
        using reference       = CS_CLASS##View<CS_CLASS>;       \
        using const_reference = CS_CLASS##View<const CS_CLASS>; \
    };                                                          \
    template<>                                                  \
    struct ChemicalSystemClassTraits<const CS_CLASS> {          \
        using value_type      = CS_CLASS;                       \
        using reference       = CS_CLASS##View<const CS_CLASS>; \
        using const_reference = CS_CLASS##View<const CS_CLASS>; \
    }

CS_TRAITS(Nucleus);
CS_TRAITS(Nuclei);

#undef CS_TRAITS

} // namespace chemist
