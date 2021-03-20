#pragma once
#include "libchemist/point_charge/point_charge.hpp"
#include <vector>

namespace libchemist::field {

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
    /// Type used to store scalar components of the electric field
    using scalar_type = double;

    /// Type used for indexing and offsets
    using size_type = std::size_t;

    /// Type used to store point charges
    using charge_type = PointCharge<scalar_type>;

    void add_charge(charge_type q) { m_charges_.emplace_back(std::move(q)); }
    auto& charge(size_type i) { return m_charges_.at(i); }
    const auto& charge(size_type i) const { return m_charges_.at(i); }
    size_type ncharges() const noexcept { return m_charges_.size(); }

private:
    std::vector<charge_type> m_charges_;
};

} // namespace libchemist::field