#pragma once
#include "libchemist/basis_set/gaussian_center.hpp"
#include "libchemist/types.hpp"
#include <utilities/iter_tools/range_container.hpp>

namespace libchemist::basis_set::detail_ {

/**
 *
 * This class basically works like GaussianCenter_ class except up one more
 * level.
 *
 * @tparam T
 */
template<typename T = double>
class AOBasisSetPIMPL {
public:
    AOBasisSetPIMPL() : m_shell_off_(1, 0), m_pps_(1, 0) {}

    type::size size() const noexcept { return m_centers_.size(); }

    void add_shell(type::coord<T> r0, type::angular_momentum l,
                   const std::vector<T>& cs, const std::vector<T>& as,
                   bool pure) {
        if(size() == 0 || m_centers_.back() != r0) { // New center
            m_centers_.push_back(r0);
            m_pps_.push_back(m_pps_.back());
            m_shell_off_.push_back(m_shell_off_.back());
        }
        m_shell_off_.back() += 1;
        m_ls_.push_back(l);
        m_pure_.push_back(pure);
        m_pps_.back() += cs.size();
        for(const auto& x : cs) m_cs_.push_back(x);
        for(const auto& x : as) m_as_.push_back(x);
    }

    auto operator[](type::size i) {
        const auto boff    = m_shell_off_[i];
        const auto eoff    = m_shell_off_[i + 1];
        const auto nshells = eoff - boff;
        const auto pcs     = &m_cs_;
        const auto pas     = &m_as_;
        using namespace utilities::detail_;
        return make_gaussian_center(
          &m_centers_[i],
          RangeContainer{m_ls_.begin() + boff, m_ls_.begin() + eoff, nshells},
          RangeContainer{m_pure_.begin() + boff, m_pure_.begin() + eoff,
                         nshells},
          RangeContainer{m_pps_.cbegin() + boff, m_pps_.cbegin() + eoff,
                         nshells},
          pcs, pas);
    }

    auto operator[](type::size i) const {
        const auto boff    = m_shell_off_[i];
        const auto eoff    = m_shell_off_[i + 1];
        const auto nshells = eoff - boff;
        using namespace utilities::detail_;
        return make_gaussian_center(
          &m_centers_[i],
          RangeContainer{m_ls_.begin() + boff, m_ls_.begin() + eoff, nshells},
          RangeContainer{m_pure_.begin() + boff, m_pure_.begin() + eoff,
                         nshells},
          RangeContainer{m_pps_.cbegin() + boff, m_pps_.cbegin() + eoff,
                         nshells},
          &m_cs_, &m_as_);
    }

private:
    /// The centers' centers
    std::vector<type::coord<T>> m_centers_;
    /// i/i+1 element is where center i's shells start/end
    std::vector<type::size> m_shell_off_;
    /// The angular momentum of each shell
    std::vector<type::angular_momentum> m_ls_;
    /// The purity of each shell
    std::vector<bool> m_pure_;
    /// i/i+1 element is where shell i's primitives start/end
    std::vector<type::size> m_pps_;
    /// The contraction coefficient of each primitive
    std::vector<T> m_cs_;
    /// The exponents of each primitive
    std::vector<T> m_as_;
};

} // namespace libchemist::basis_set::detail_
