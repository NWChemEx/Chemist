#pragma once
#include "libchemist/operator/operator_base.hpp"
#include <utilities/type_traits/parameter_pack_traits.hpp>

namespace libchemist {
class Electron;
class Molecule;
namespace detail_ {

/** @brief Code factorization for implementing an operator.
 *
 *  Many of the operators are more-or-less strong types aside from the
 *  parameters associated with the "particles" involved in the term. This class
 *  provides code factorization for those
 *
 *  @note Here we use the term "particles" very loosely and include traditional
 *  particles like electrons and nuclei, but also things like electrostatic
 *  fields. Basically particles are the entities which are interacting in the
 *  operator.
 */
template<template<typename...> typename DerivedClass, typename... Particles>
class OperatorImpl : public OperatorBase {
private:
    using Nuclei       = Molecule;
    using derived_type = DerivedClass<Particles...>;

    template<typename P>
    static constexpr auto count_particle_v =
      utilities::type_traits::parameter_pack_count_derived_type_v<P,
                                                                  Particles...>;

public:
    static constexpr auto n_electrons = count_particle_v<Electron>;

    static constexpr auto n_nuclei = count_particle_v<Nuclei>;

    OperatorImpl() = default;

    template<typename... Inputs>
    OperatorImpl(Inputs&&... inputs) :
      m_particles_(std::make_tuple(std::forward<Inputs>(inputs)...)) {}

    template<typename... OtherParticles>
    bool operator==(const DerivedClass<OtherParticles...>& rhs) const;

    template<typename... OtherParticles>
    bool operator!=(const DerivedClass<OtherParticles...>& other) const;

    template<std::size_t N>
    const auto& at() const {
        return std::get<N>(m_particles_);
    }

protected:
    virtual bool is_equal_impl(const OperatorBase& rhs) const noexcept override;
    virtual void hash_impl(pluginplay::Hasher&) const override {}

private:
    std::tuple<Particles...> m_particles_;
};

#define OPERATOR_IMPL OperatorImpl<DerivedClass, Particles...>

template<template<typename...> typename DerivedClass, typename... Particles>
template<typename... OtherParticles>
bool OPERATOR_IMPL::operator==(
  const DerivedClass<OtherParticles...>& rhs) const {
    if constexpr(!std::is_same_v<decltype(*this), decltype(rhs)>) {
        return false;
    } else {
        return m_particles_ == rhs.m_particles_;
    }
}

template<template<typename...> typename DerivedClass, typename... Particles>
template<typename... OtherParticles>
bool OPERATOR_IMPL::operator!=(
  const DerivedClass<OtherParticles...>& rhs) const {
    return !((*this) == rhs);
}

template<template<typename...> typename DerivedClass, typename... Particles>
bool OPERATOR_IMPL::is_equal_impl(const OperatorBase& rhs) const noexcept {
    auto ptr = dynamic_cast<const derived_type*>(&rhs);
    return ptr ? (*this == *ptr) : false;
}

#undef OPERATOR_IMPL

} // namespace detail_

} // namespace libchemist
