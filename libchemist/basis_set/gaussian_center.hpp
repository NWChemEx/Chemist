#include "libchemist/basis_set/detail_/gaussian_center_traits.hpp"
#include "libchemist/basis_set/detail_/holder.hpp"
#include "libchemist/basis_set/gaussian_shell.hpp"

namespace libchemist::basis_set {

template<typename Traits>
class GaussianCenter_ {
public:
    template<typename CenterType, typename ShellArray>
    constexpr GaussianCenter_(CenterType&& r0, ShellArray&& shells) noexcept :
      m_cs_(std::forward<ShellArray>(shells)),
      m_center_(std::forward<CenterType>(r0)) {}

    constexpr type::size size() const noexcept { return m_cs_->size(); }

    constexpr auto& center() noexcept { return *m_center_; }
    constexpr const auto& center() const noexcept { return *m_center_; }

    constexpr auto& operator[](type::size i) noexcept { return (*m_cs_)[i]; }
    constexpr const auto& operator[](type::size i) const noexcept {
        return (*m_cs_)[i];
    }

private:
    detail_::holder<typename Traits::shell_array> m_cs_;
    detail_::holder<typename Traits::center_type> m_center_;
};

template<typename CenterType, typename ShellArray>
constexpr auto make_gaussian_center(CenterType&& r0,
                                    ShellArray&& shells) noexcept {
    using clean_center = std::remove_reference_t<CenterType>;
    using clean_shells = std::remove_reference_t<ShellArray>;
    using traits = detail_::GaussianCenterTraits<clean_center, clean_shells>;
    return GaussianCenter_<traits>(std::forward<CenterType>(r0),
                                   std::forward<ShellArray>(shells));
}

} // namespace libchemist::basis_set
