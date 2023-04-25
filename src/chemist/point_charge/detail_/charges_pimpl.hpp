// #pragma once
// #include <chemist/point/detail_/point_set_pimpl.hpp>
// #include <chemist/point_charge/charges.hpp>

// namespace chemist::detail_ {

// template<typename T>
// class ChargesPIMPL : public PointSetPIMPL<T> {
// private:
//     /// Type of *this
//     using my_type = ChargesPIMPL<T>;

//     /// Type of a PointSetViewPIMPL powered by *this
//     using point_set_view_pimpl = PointSetViewPIMPL<const my_type>;

// public:
//     /// Type we are implementing
//     using parent_type = Charges<T>;

//     /// Reuse parent class's types
//     ///@{
//     using pimpl_pointer   = typename base_type::pimpl_pointer;
//     using value_type      = typename parent_type::value_type;
//     using reference       = typename parent_type::reference;
//     using const_reference = typename parent_type::const_reference;
//     using size_type       = typename parent_type::size_type;
//     ///@}

//     reference at(size_type i) { return at_(i); }
//     const_reference at(size_type i) const { return at_(i); }

//     size_type size() const noexcept { return size_(); }
//     const PointSet<T> as_point_set() { return m_points_; }

// private:
//     virtual reference at_(size_type i) { return m_charges_.at(i); }
//     virtual const_reference at_(size_type i) const { return m_charges_.at(i);
//     }

//     virtual size_type_(size_type i) const { return m_charges_.size(); }

//       PointSet<T> m_points_;
//     std::vector<value_type> m_charges_;
// };

// } // namespace chemist::detail_
