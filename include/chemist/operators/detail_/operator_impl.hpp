#pragma once
#include "chemist/operators/detail_/operator_base.hpp"
#include <utilities/type_traits/parameter_pack_traits.hpp>

namespace chemist {
class Molecule;
}

namespace chemist::operators::detail_ {

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
 *                       and is being implemented by it. DerivedClass must be
 *                       templated on one or more types.
 *  @tparam Particles    The types of the physical objects which are interacting
 *                       in this operator. These are the template type
 *                       parameters which will be provided to DerivedClass and
 *                       are expected to be types like: Electron, ManyElectrons,
 *                       ElectronDensity, Nuclei, etc.
 */
template<template<typename...> typename DerivedClass, typename... Particles>
class OperatorImpl : public OperatorBase {
private:
    /// Type of the object which holds a collection of Nuclei
    using Nuclei = Molecule;

    /// Type of the class which derives from this class
    using derived_type = DerivedClass<Particles...>;

public:
    /// Type of a tuple filled with the Particles' types
    using particle_type = std::tuple<Particles...>;

    template<template<typename...> typename OtherDerived, typename... OtherPs>
    bool operator==(const OperatorImpl<OtherDerived, OtherPs...>& rhs) const;

    template<template<typename...> typename OtherDerived, typename... OtherPs>
    bool operator!=(const OperatorImpl<OtherDerived, OtherPs...>& rhs) const;

    /** @brief Returns the @p N-th particle in the interaction.
     *
     *  @tparam N Which particle to retrieve. @p N must be in the range
     *            [0, n_body).
     *
     *  @return A read-only reference to the @p N-th particle stored in the
     *          operator.
     *
     *  @throw None No throw guarantee.
     */
    template<std::size_t N>
    const auto& at() const {
        return std::get<N>(m_particles_);
    }

protected:
    OperatorImpl()                        = default;
    OperatorImpl(const OperatorImpl& rhs) = default;
    OperatorImpl(OperatorImpl&& rhs)      = default;

    OperatorImpl& operator=(const OperatorImpl&) = default;
    OperatorImpl& operator=(OperatorImpl&&) = default;

    OperatorImpl(Particles... inputs) :
      m_particles_(std::make_tuple(std::move(inputs)...)) {}

    bool is_equal_impl(const OperatorBase& rhs) const noexcept override;
    std::unique_ptr<OperatorBase> clone_impl() const override;
    void hash_impl(chemist::detail_::Hasher& h) const override;
    std::string as_string_impl() const override;

private:
    /// The particle instances involved in this operator
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
std::unique_ptr<OperatorBase> OPERATOR_IMPL::clone_impl() const {
    const auto& derived = static_cast<const derived_type&>(*this);
    return std::make_unique<derived_type>(derived);
}

template<template<typename...> typename DerivedClass, typename... Particles>
void OPERATOR_IMPL::hash_impl(chemist::detail_::Hasher& h) const {
    h(m_particles_);
}

template<template<typename...> typename DerivedClass, typename... Particles>
std::string OPERATOR_IMPL::as_string_impl() const {
    return "O\u0302";
}

#undef OPERATOR_IMPL

} // namespace chemist::operators::detail_
