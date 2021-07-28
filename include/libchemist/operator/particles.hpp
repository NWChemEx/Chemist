#pragma once
#include <utilities/type_traits/parameter_pack_traits.hpp>

namespace libchemist {

/// Strong type representing an electron.
struct Electron {};
/// Strong type representing a dipole field.
struct DipoleField {};
/// Strong type representing a quadrupole field.
struct QuadrupoleField {};
/// Strong type representing a octupole field.
struct OctupoleField {};

/// Counts the number of electrons in a parameter pack
template<typename... Particles>
inline static constexpr std::size_t n_electrons_v =
  utilities::type_traits::parameter_pack_count_derived_type_v<Electron,
                                                              Particles...>;

/// Counts the number of nuclei in a parameter pack
template<typename... Particles>
inline static constexpr std::size_t n_nuclei_v =
  utilities::type_traits::parameter_pack_count_derived_type_v<Nucleus,
                                                              Particles...>;
} // namespace libchemist
