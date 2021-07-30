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
 *  provides code factorization for comparing the "particles" in the term.
 *
 *  @note Here we use the term "particles" very loosely and include traditional
 *  particles like Electrons and nuclei, but also things like electrostatic
 *  fields. Basically particles are the entities which are interacting in the
 *  operator.
 *
 *  @tparam DerivedClass The name of the operator which derives from this class
 *                       and is being implemented by it. DerivedClass may be
 *                       templated on zero or more types
 */
template<template<typename...> typename DerivedClass, typename... Particles>
class OperatorImpl : public OperatorBase {
private:
    /// Type of the object which holds a collection of Nuclei
    using Nuclei = Molecule;

    /// Type of the class which derives from this class
    using derived_type = DerivedClass<Particles...>;

    /// Counts the number of times @P appears in Particles.
    template<typename P>
    static constexpr auto count_particle_v =
      utilities::type_traits::parameter_pack_count_derived_type_v<P,
                                                                  Particles...>;

public:
    /// Type of a tuple filled with the Particles' types
    using particle_type = std::tuple<Particles...>;

    /// The total number of particles involved in this operator
    static constexpr auto n_body = sizeof...(Particles);

    /// The total number of Electrons involved in this interaction
    static constexpr auto n_electrons = count_particle_v<Electron>;

    /// The total number of nuclei involved in this interaction
    static constexpr auto n_nuclei = count_particle_v<Nuclei>;

    template<template<typename...> typename OtherDerived, typename... OtherPs>
    bool operator==(const OperatorImpl<OtherDerived, OtherPs...>& rhs) const;

    template<template<typename...> typename OtherDerived, typename... OtherPs>
    bool operator!=(const OperatorImpl<OtherDerived, OtherPs...>& rhs) const;

    template<std::size_t N>
    const auto& at() const {
        return std::get<N>(m_particles_);
    }

protected:
    OperatorImpl() = default;

    template<typename... Inputs>
    OperatorImpl(Inputs&&... inputs) :
      m_particles_(std::make_tuple(std::forward<Inputs>(inputs)...)) {}

    virtual bool is_equal_impl(const OperatorBase& rhs) const noexcept override;
    virtual void hash_impl(pluginplay::Hasher& h) const override;

private:
    std::tuple<Particles...> m_particles_;
};

#define OPERATOR_IMPL OperatorImpl<DerivedClass, Particles...>

template<template<typename...> typename DerivedClass, typename... Particles>
template<template<typename...> typename OtherDerived, typename... OtherPs>
bool OPERATOR_IMPL::operator==(
  const OperatorImpl<OtherDerived, OtherPs...>& rhs) const {
    if constexpr(!std::is_same_v<decltype(*this), decltype(rhs)>) {
        return false;
    } else {
        return m_particles_ == rhs.m_particles_;
    }
}

template<template<typename...> typename DerivedClass, typename... Particles>
template<template<typename...> typename OtherDerived, typename... OtherPs>
bool OPERATOR_IMPL::operator!=(
  const OperatorImpl<OtherDerived, OtherPs...>& rhs) const {
    return !((*this) == rhs);
}

template<template<typename...> typename DerivedClass, typename... Particles>
bool OPERATOR_IMPL::is_equal_impl(const OperatorBase& rhs) const noexcept {
    auto ptr = dynamic_cast<const derived_type*>(&rhs);
    return ptr ? (*this == *ptr) : false;
}

template<template<typename...> typename DerivedClass, typename... Particles>
void OPERATOR_IMPL::hash_impl(pluginplay::Hasher& h) const {
    h(m_particles_);
}

#undef OPERATOR_IMPL

} // namespace detail_

} // namespace libchemist
