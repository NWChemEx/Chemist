#pragma once
#include "libchemist/types.hpp"

struct GaussianCenterAlias {
public:
    constexpr auto& coord() noexcept;
    constexpr const auto& coord() const noexcept;

    GaussianShell shell(type::size i);
    GaussianAO ao(type::size i, type::size j);
    Gaussian prim(type::size i, type::size j, type::size k);

private:
    type::size m_index_;
};

struct GaussianShellAlias {
public:
    constexpr const auto& coord() const noexcept;

private:
    type::size m_coord_;
    type::size m_index_;
};

struct GaussianAOView {
public:
    constexpr const auto& coord() const noexcept;

private:
    type::size m_coord_;
    type::size m_shell_;
    type::size m_index_;
};
}
; // End namespace
