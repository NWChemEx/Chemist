#pragma once
#include "libchemist/operator/operator_base.hpp"

namespace libchemist {
namespace detail_ {

/** @brief Code factorization for implementing an operator.
 *
 */
template<template<typename...> typename DerivedClass, typename... Particles>
class OperatorImpl : public Operator {
private:
    using derived_type = DerivedClass<Particles...>;

    template<typename P>
    static constexpr auto count_particle_v =
      utilities::type_traits::parameter_pack_count_derived_type_v<P,
                                                                  Particles...>;

public:
    static constexpr auto n_electrons = count_particle_v<Electron>;

    static constexpr auto n_nuclei = count_particle_v<Nuceli>;

    template<typename... OtherParticles>
    bool operator==(const DerivedClass<OtherParticles...>& rhs) const;

    template<typename... OtherParticles>
    bool operator!=(const DerivedClass<OtherParticles...>& other) const;

protected:
    virtual bool is_equal_impl(const Operator& rhs) const noexcept override;
    virtual void hash_impl(pluginplya::Hasher&) const override {}

private:
    std::tuple<Particles...> m_particles_;
};

template<template<typename...> typename DerivedClass, typename... Particles>
template<typename... OtherParticles>
bool StatelessOperator<DerivedClass, Particles...>::operator==(
  const DerivedClass<OtherParticles...>& rhs) const {
    if constexpr(!std::is_same_v<decltype(*this), decltype(rhs)>) {
        return false;
    } else {
        return m_particles_ == rhs.m_particles_;
    }
}

template<template<typename...> typename DerivedClass, typename... Particles>
template<typename... OtherParticles>
bool StatelessOperator<DerivedClass, Particles...>::operator!=(
  const DerivedClass<OtherParticles...>& rhs) const {
    return !((*this) == rhs);
}

template<template<typename...> typename DerivedClass, typename... Particles>
bool StatelessOperator<DerivedClass, Particles...>::is_equal_impl(
  const Operator& rhs) const noexcept {
    auto ptr = dynamic_cast<const DerivedClass*>(rhs);
    return ptr ? (*this == *ptr) : false;
}

} // namespace detail_

} // namespace libchemist
