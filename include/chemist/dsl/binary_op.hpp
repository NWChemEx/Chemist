#pragma once
#include <chemist/dsl/term.hpp>
#include <type_traits>

namespace chemist::dsl {

template<typename DerivedType, typename LHSType, typename RHSType>
class BinaryOp : public Term<DerivedType> {
private:
    template<typename T>
    static constexpr bool is_const_v = std::is_const_v<LHSType>;

    template<typename T>
    using value_type_ = std::decay_t<T>;

    template<typename T>
    using const_reference_ = const value_type_<T>&;

    template<typename T>
    using reference_ =
      std::conditional_t<is_const_v<T>, const_reference_<T>&, value_type_<T>>;

public:
    using lhs_type            = value_type_<LHSType>;
    using lhs_reference       = reference_<LHSType>;
    using const_lhs_reference = const_reference_<LHSType>;

    using rhs_type            = value_type_<RHSType>;
    using rhs_reference       = reference_<RHSType>;
    using const_rhs_reference = const_reference_<RHSType>;

    BinaryOp(lhs_reference l, rhs_reference r) : m_lhs_(l), m_rhs_(r) {}

    lhs_reference lhs() { return m_lhs_; }
    const_lhs_reference lhs() const { return m_lhs_; }

    rhs_reference rhs() { return m_rhs_; }
    const_rhs_reference rhs() const { return m_rhs_; }

    template<typename DerivedType2, typename LHSType2, typename RHSType2>
    bool operator==(
      const BinaryOp<DerivedType2, LHSType2, RHSType2>& other) const noexcept {
        constexpr auto l_same = std::is_same_v<lhs_type, value_type_<LHSType2>>;
        constexpr auto r_same = std::is_same_v<rhs_type, value_type_<RHSType2>>;
        if constexpr(l_same && r_same) {
            return std::tie(lhs(), rhs()) == std::tie(other.lhs(), other.rhs());
        } else {
            return false;
        }
    }

private:
    lhs_reference m_lhs_;
    rhs_reference m_rhs_;
};

} // namespace chemist::dsl