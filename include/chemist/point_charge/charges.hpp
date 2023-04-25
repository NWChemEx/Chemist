// #pragma once
// #include <chemist/point_charge/point_charge.hpp>

// namespace chemist {
// namespace detail_ {
// template<typename T>
// class ChargesPIMPL;
// }

// template<typename T>
// class Charges {
// public:
//     /// The type of the PIMPL
//     using pimpl_type = detail_::ChargesPIMPL<T>;

//     /// The type of a pointer to a PIMPL
//     using pimpl_pointer = std::unique_ptr<pimpl_type>;

//     /// The elements in the container
//     using value_type = PointCharge<T>;

//     /// Read/write reference to an element
//     using reference = value_type&;

//     /// Read-only reference to an element
//     using const_reference = const value_type&;

//     /// Integral type used for indexing
//     using size_type = std::size_t;

//     Charges() noexcept;
//     ~Charges() const noexcept;

//     reference at(size_type i);

//     const_reference at(size_type i);

// private:
//     pimpl_pointer m_pimpl_;
// };

// } // namespace chemist
