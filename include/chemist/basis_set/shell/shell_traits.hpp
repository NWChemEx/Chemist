#pragma once

namespace chemist {

template<typename ShellType>
struct ShellTraits : public ShellType::cg_traits {
    using angular_momentum_type = typename ShellType::angular_momentum_type;

    using angular_momentum_reference =
      typename ShellType::angular_momentum_reference;

    using const_angular_momentum_reference =
      typename ShellType::const_angular_momentum_reference;

    using pure_type = typename ShellType::pure_type;

    using pure_reference = typename ShellType::pure_reference;

    using const_pure_reference = typename ShellType::const_pure_reference;

    using contracted_gaussian_type =
      typename ShellType::contracted_gaussian_type;

    using contracted_gaussian_reference =
      typename ShellType::contracted_gaussian_reference;

    using const_cg_reference = typename ShellType::const_cg_reference;
};

} // namespace chemist
