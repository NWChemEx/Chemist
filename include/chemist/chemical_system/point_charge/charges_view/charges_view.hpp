#pragma once
#include <chemist/chemical_system/point_charge/charges.hpp>
#include <chemist/traits/point_charge_traits.hpp>
#include <utilities/containers/indexable_container_base.hpp>

namespace chemist {
namespace detail_ {

template<typename ChargesType>
class ChargesViewPIMPL;

}

template<typename ChargesType>
class ChargesView
  : public utilities::IndexableContainerBase<ChargesView<ChargesType>> {
private:
    /// Type of *this
    using my_type = ChargesView<ChargesType>;

    /// Type of the base class
    using base_type = utilities::IndexableContainerBase<my_type>;

public:
    using pimpl_type = detail_::ChargesViewPIMPL<ChargesType>;

    using pimpl_pointer = std::unique_ptr<pimpl_type>;

    using charges_traits = ChemistClassTraits<ChargesType>;

    using point_set_traits = typename charges_traits::point_set_traits;

    using point_set_type = typename point_set_traits::value_type;

    using point_set_reference = typename point_set_traits::view_type;

    using point_charge_traits = typename charges_traits::point_charge_traits;

    using reference = typename point_charge_traits::view_type;

    using const_reference = typename point_charge_traits::const_view_type;

    using charge_pointer = typename point_charge_traits::charge_pointer;

    using typename base_type::size_type;

    /** @brief Aliases an empty Charges object
     *
     * @throw None No throw guarantee
     */
    ChargesView() noexcept;

    ChargesView(const ChargesView& other);

    ChargesView& operator=(const ChargesView& rhs);

    ChargesView(ChargesView&& other) noexcept;

    ChargesView& operator=(ChargesView&& rhs) noexcept;

    /// Default no-throw dtor
    ~ChargesView() noexcept;

private:
    // Allows base class to access implementations
    friend base_type;

    /// Used internally to check if *this has a PIMPL or not
    bool has_pimpl_() const noexcept;

    /// Used internally to wrap the logic associated with cloning the PIMPL
    pimpl_pointer clone_pimpl_() const;

    /// Implements base_type::operator[]() and base_type::at()
    reference at_(size_type i);

    /// Implements base_type::operator[]() const and base_type::at() const
    const_reference at_(size_type i) const;

    /// Implements base_type::size()
    size_type size_() const noexcept;

    /// The actual object holding the state of *this
    pimpl_pointer m_pimpl_;
};

extern template class ChargesView<Charges<float>>;
extern template class ChargesView<const Charges<float>>;
extern template class ChargesView<Charges<double>>;
extern template class ChargesView<const Charges<double>>;

} // namespace chemist
