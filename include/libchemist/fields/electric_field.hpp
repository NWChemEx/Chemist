#pragma once
#include "libchemist/point/point.hpp"

namespace libchemist::field {

template<typename ScalarType = double>
class PointCharge : public Point<ScalarType> {
public:
    using scalar_type = double;

    PointCharge(scalar_type q, scalar_type x, scalar_type y, scalar_type z) :
      Point(x, y, z), m_q_(q) {}

    scalar_type& charge() { return m_q_; }
    const scalar_type& charge() const { return m_q_; }

private:
    /// The charge associated with this point charge
    scalar_type m_q_;
};

/** @brief Class that models an electric field.
 *
 *  At the moment I'm not exactly sure what I want from this class beyond it
 *  needs to be able to hold point charges. Realistically the nuclei also create
 *  an electric field so it may make sense to use the class for them too. Then
 *  there's the possibility of going beyond point charges to dipoles, but also
 *  possibly to full electronic densities...So consider the API of this class
 *  subject to change without notice.
 *
 */
class Electric {
public:
    using scalar_type = double;
    using size_type   = std::size_t;
    using charge_type = PointCharge<scalar_type>;

    void add_charge(charge_type q) { m_charges_.emplace_back(std::move(q)); }
    auto& charge(size_type i) { return m_charges_.at(i); }
    const auto& charge(size_type i) const { return m_charges_at(i); }
    size_type ncharges() const noexcept { return m_charges_; }

private:
    std::vector<charge_type> m_charges_;
};

} // namespace libchemist::field