#pragma once
#include "chemist/electrons/electrons.hpp"
#include "chemist/enums.hpp"
#include "chemist/operators/detail_/operator_base.hpp"

namespace chemist::operators {

/** @brief Models a multipole/
 *
 *  @tparam L The
 *  @tparam ParticleType The type of the particle.
 *  @tparam Basis The type of the basis set shell type.
 *  @tparam Gauge The type of the gauge.
 */
template<std::size_t L, typename ParticleType,
         ShellType Basis = ShellType::cartesian,
         GaugeType Gauge = GaugeType::length>
class Multipole : public detail_::OperatorBase {
private:
    using my_type = Multipole<L, ParticleType, Basis, Gauge>;

public:
    /// Type of a point in 3-dimensional space
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

    /** @brief Returns the gauge origin of the multipole.
     *
     *  @return A read-only reference to the origin.
     *
     *  @throw None No throw guarantee.
     */
    inline const auto& gauge_origin() const { return gauge_origin_; }

    /** @brief Returns the gauge origin of the multipole.
     *
     *  @return A read/write reference to the origin.
     *
     *  @throw None No throw guarantee.
     */
    inline auto& gauge_origin() { return gauge_origin_; }

protected:
    inline void hash_impl(chemist::detail_::Hasher& h) const override {
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

    /// Gives a more traditional representation than the default implementation
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
using ElectricDipole = Multipole<1, chemist::Electron>;
/// Electric quadrupole operator type
using ElectricQuadrupole = Multipole<2, chemist::Electron>;
/// Electric octupole operator type
using ElectricOctupole = Multipole<3, chemist::Electron>;

} // namespace chemist::operators
