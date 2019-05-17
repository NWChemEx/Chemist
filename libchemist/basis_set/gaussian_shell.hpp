#include "libchemist/basis_set/detail_/gaussian_shell_traits.hpp"
#include "libchemist/basis_set/detail_/holder.hpp"
#include "libchemist/basis_set/gaussian_ao.hpp"
#include <utilities/iter_tools/range_container.hpp>

namespace libchemist::basis_set {

template<typename Traits>
class GaussianShell_ {
public:
    template<typename T, typename U, typename V, typename W>
    constexpr GaussianShell_(T&& l, U&& cs, V&& as, W&& r0) noexcept :
      m_l_(std::forward<T>(l)),
      m_cs_(std::forward<U>(cs)),
      m_as_(std::forward<V>(as)),
      m_center_(std::forward<W>(r0)) {}

    constexpr type::size size() const noexcept {
        if constexpr(pure()) return 2 * l() + 1;
        return binomial_coef_(l() + 2ul, 2ul);
    }

    constexpr auto& l() noexcept { return *m_l_; }
    constexpr const auto l() const noexcept { return *m_l_; }

    constexpr auto& center() noexcept { return *m_center_; }
    constexpr const auto& center() const noexcept { return *m_center_; }

    static constexpr bool pure() noexcept { return Traits::is_pure; }

    constexpr auto operator[](type::size) const noexcept {
        return make_gaussian_ao(
          utilities::detail_::RangeContainer{m_cs_->begin(), m_cs_->end(),
                                             size()},
          utilities::detail_::RangeContainer{m_as_->begin(), m_as_->end(),
                                             size()},
          center());
    }

private:
    constexpr type::size binomial_coef_(type::size n, type::size m) const
      noexcept {
        if(m == 0) return 1ul;
        if(n == 0) return 0ul;
        return binomial_coef_(n - 1, m) + binomial_coef_(n - 1, m - 1);
    }

    detail_::holder<typename Traits::am_type> m_l_;
    detail_::holder<typename Traits::coef_array> m_cs_;
    detail_::holder<typename Traits::exp_array> m_as_;
    detail_::holder<typename Traits::center_type> m_center_;
};

template<typename T, type::size N, bool IsPure = true>
using StaticGaussianShell =
  GaussianShell_<detail_::StaticGuassianShellTraits<T, N, IsPure>>;

template<bool IsPure, typename CoefArray, typename ExpArray,
         typename CenterType>
constexpr auto make_gaussian_shell(type::angular_momentum l, CoefArray&& cs,
                                   ExpArray&& as, CenterType&& r0) noexcept {
    using clean_coefs  = std::remove_reference_t<CoefArray>;
    using clean_exps   = std::remove_reference_t<ExpArray>;
    using clean_center = std::remove_reference_t<CenterType>;
    using traits =
      detail_::GaussianShellTraits_<clean_coefs, clean_exps, clean_center>;
    return GaussianShell_<traits>(l, std::forward<CoefArray>(cs),
                                  std::forward<ExpArray>(as),
                                  std::forward<CenterType>(r0));
}

} // namespace libchemist::basis_set
