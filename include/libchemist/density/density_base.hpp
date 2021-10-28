#pragma once
#include "libchemist/orbital_space/ao_space.hpp"
#include "libchemist/types.hpp"

namespace libchemist {

/** @brief Class describing the interface of a density
 *
 *  This class lays out the expected behavior of a density class. Specifically
 *  - access to the value of the density
 *  - access to the space the density is formed in
 *  - hashing the density instance
 *
 *  @tparam Particles The types of the particles described by this density. At
 *                    the moment the types are used purely for strong typing
 *                    purposes. Expected to be unqualified types.
 *
 */
template<typename... Particles>
class DensityBase {
public:
    /// Type used to represent the numeric form of the density
    using value_type = type::tensor;

    /// Type used to represent the basis set
    using aos_type = orbital_space::AOSpaceD;

    /** @brief Provides read-only access to the orbital basis set used to define
     *         the density matrix.
     *
     *  @return The orbital basis the density is formed in.
     *
     *  @throw None No throw guarantee.
     */
    const auto& basis_set() const { return basis_set_impl(); }

    /** @brief Provides read-only access to the density tensor.
     *
     *  @return The value of the density matrix.
     *
     *  @throw None No throw guarantee.
     */
    const auto& value() const { return m_density_; }

    /** @brief Hash the DensityBase object
     *
     *  @throw None No throw guarantee.
     */
    void hash(pluginplay::Hasher& h) const { hash_impl(h); }

    /** @brief Non-polymorphic equality comparison of DensityBase instances
     *
     *  Two DensityBase instances are equal if they contain the same exact
     *  value.
     *
     *  @param[in] other Density instance to compare
     *  @return    True if the DensityBase instances are equal and false
     *             otherwise.
     *
     *  @throw None No throw guarantee.
     */
    template<typename... OtherPs>
    bool operator==(const DensityBase<OtherPs...>& other) const;

    /** @brief Non-polymorphic inequality comparison of DensityBase instances
     *
     *  Two DensityBase instances are different if they contain different
     *  value.
     *
     *  @param[in] other Density instance to compare
     *  @return    False if theDensityBase instances are equal and true
     *             otherwise.
     *
     *  @throw None No throw guarantee.
     */
    template<typename... OtherPs>
    bool operator!=(const DensityBase<OtherPs...>& other) const;

    /// Polymorphic defaulted no-throw dtor
    virtual ~DensityBase() = default;

protected:
    /// Defaulted default noexcept ctor
    DensityBase() = default;
    /// Explicit ctor with non-default value
    explicit DensityBase(value_type rho) : m_density_(std::move(rho)) {}
    /// Defaulted copy ctor, protected to avoid slicing
    DensityBase(const DensityBase&) = default;
    /// Defaulted move ctor, protected to avoid slicing
    DensityBase(DensityBase&&) noexcept = default;
    /// Defaulted copy assignment, protected to avoid slicing
    DensityBase& operator=(const DensityBase&) = default;
    /// Defaulted move assignment, protected to avoid slicing
    DensityBase& operator=(DensityBase&&) noexcept = default;

    /// Derived implementation to get basis set
    virtual const aos_type& basis_set_impl() const = 0;
    /// Derived implementation of Hash function.
    virtual void hash_impl(pluginplay::Hasher& h) const = 0;

private:
    value_type m_density_;
};

// -----------------------------------------------------------------------------
// -------------------------- Inline Implementations ---------------------------
// -----------------------------------------------------------------------------

#define DENSITY_BASE DensityBase<Particles...>

template<typename... Particles>
template<typename... OtherPs>
inline bool DENSITY_BASE::operator==(
  const DensityBase<OtherPs...>& other) const {
    if constexpr(std::is_same_v<decltype(*this), decltype(other)>) {
        return *this->value() == other.value();
    } else {
        return false;
    }
}

template<typename... Particles>
template<typename... OtherPs>
inline bool DENSITY_BASE::operator!=(
  const DensityBase<OtherPs...>& other) const {
    return !((*this) == other);
}

#undef DENSITY_BASE

} // namespace libchemist