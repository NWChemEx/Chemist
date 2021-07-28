#pragma once
#include "libchemist/enums.hpp"
#include "libchemist/operator/operator_base.hpp"
namespace libchemist {

/// Multipole operator
template<std::size_t L, typename ParticleType,
         ShellType Basis = ShellType::cartesian,
         GaugeType Gauge = GaugeType::length>
class Multipole : public OperatorBase {
private:
    using my_type = Multipole<L, ParticleType, Basis, Gauge>;

public:
    using point_type = Point<double>;

    static constexpr auto n_electrons = detail_::n_electrons_v<ParticleType>;
    static constexpr auto n_nuclei    = detail_::n_nuclei_v<ParticleType>;

    // static constexpr std:size_t op_size =
    //  Basis == ShellType::cartesian ? ((L+1)*(L+2)/2) : (2*L+1);

    inline bool operator==(const Multipole& other) const {
        return gauge_origin_ == other.gauge_origin_;
    }
    inline bool operator!=(const Multipole& other) const {
        return not(*this == other);
    }

    Multipole(point_type p) : gauge_origin_(p) {}
    Multipole() : Multipole(point_type(0., 0., 0.)) {}

    Multipole(const Multipole& other) : gauge_origin_(other.gauge_origin_) {}
    Multipole(Multipole&& other) noexcept :
      gauge_origin_(std::move(other.gauge_origin_)) {}

    ~Multipole() noexcept = default;

    inline const auto& gauge_origin() const { return gauge_origin_; }
    inline auto& gauge_origin() { return gauge_origin_; }

protected:
    inline void hash_impl(sde::Hasher& h) const override {
        return h(gauge_origin);
    }

    inline bool is_equal_impl(
      const OperatorBase& other) const noexcept override {
        auto ptr = dynamic_cast<const my_type*>(&other);
        return (ptr) ? *this == *ptr : false;
    }

private:
    /// Gauge origin for the multipole operator
    point_type gauge_origin_;
};

/// Electric dipole operator type
using ElectricDipoleOperator = Multipole<1, Electron>;
/// Electric quadrupole operator type
using ElectricQuadrupoleOperator = Multipole<2, Electron>;
/// Electric octupole operator type
using ElectricOctupoleOperator = Multipole<3, Electron>;

} // namespace libchemist
