#pragma once
#include <chemist/chemical_system/electron/electron.hpp>
#include <chemist/quantum_mechanics/operator/detail_/operator_impl.hpp>
#include <chemist/traits/electron_traits.hpp>

namespace chemist::qm_operator {

template<typename ParticleType>
class Kinetic : public detail_::OperatorImpl<ParticleType> {
private:
    /// Type of *this
    using my_type = Kinetic<ParticleType>;

    /// Type *this directly derives from.
    using base_type = detail_::OperatorImpl<ParticleType>;

public:
    /// Pull in types from the base class
    ///@{
    using typename base_type::base_pointer;
    using typename base_type::visitor_reference;
    ///@}

    using value_type = typename base_type::template particle_type<0>;
    using reference  = typename base_type::template particle_reference<0>;
    using const_reference =
      typename base_type::template const_particle_reference<0>;

    explicit Kinetic(value_type particle = value_type{});
    ~Kinetic() noexcept = default;

    reference particle() { return base_type::template at<0>(); }
    const_reference particle() const { return base_type::template at<0>(); }

private:
    void visit_(visitor_reference visitor) const override;
    base_pointer clone_() const override;
};

extern template class Kinetic<Electron>;
extern template class Kinetic<ManyElectrons>;

} // namespace chemist::qm_operator