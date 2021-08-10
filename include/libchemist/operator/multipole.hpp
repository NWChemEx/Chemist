#pragma once
#include "libchemist/enums.hpp"
#include "libchemist/operator/detail_/operator_base.hpp"

namespace libchemist::operators {

/// Multipole operator
template<std::size_t L, typename ParticleType,
         ShellType Basis = ShellType::cartesian,
         GaugeType Gauge = GaugeType::length>
class Multipole : public detail_::OperatorBase {
private:
    using my_type = Multipole<L, ParticleType, Basis, Gauge>;

public:
    using point_type = Point<double>;

    inline bool operator==(const Multipole& other) const {
        return gauge_origin_ == other.gauge_origin_;
    }
    inline bool operator!=(const Multipole& other) const {
        return not(*this == other);
    }

    Multipole(point_type p = point_type(0., 0., 0.)) : gauge_origin_(p) {}

    Multipole(const Multipole& other)     = default;
    Multipole(Multipole&& other) noexcept = default;
    Multipole& operator=(const Multipole&) = default;
    Multipole& operator=(Multipole&&) = default;

    ~Multipole() noexcept = default;

    inline const auto& gauge_origin() const { return gauge_origin_; }
    inline auto& gauge_origin() { return gauge_origin_; }

protected:
    inline void hash_impl(pluginplay::Hasher& h) const override {
        return h(gauge_origin_);
    }

    inline bool is_equal_impl(
      const OperatorBase& other) const noexcept override {
        auto ptr = dynamic_cast<const my_type*>(&other);
        return (ptr) ? *this == *ptr : false;
    }

    std::unique_ptr<OperatorBase> clone_impl() const {
        return std::make_unique<my_type>(*this);
    }

    std::string as_string_impl() const {
        if constexpr(L == 1) {
            return "r";
        } else if constexpr(L == 2) {
            return "r²";
        } else if constexpr(L == 3) {
            return "r³";
        } else
            return OperatorBase::as_string_impl();
    }

private:
    /// Gauge origin for the multipole operator
    point_type gauge_origin_;
};

/// Electric dipole operator type
using ElectricDipole = Multipole<1, Electron>;
/// Electric quadrupole operator type
using ElectricQuadrupole = Multipole<2, Electron>;
/// Electric octupole operator type
using ElectricOctupole = Multipole<3, Electron>;

} // namespace libchemist::operators
