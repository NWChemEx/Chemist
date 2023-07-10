#pragma once
#include <chemist/basis_set2/contracted_gaussian/contracted_gaussian.hpp>
#include <chemist/basis_set2/contracted_gaussian/contracted_gaussian_traits.hpp>

namespace chemist::basis_set::detail_ {

/** @brief Implements a ContractedGaussian object.
 *
 *  @tparam PrimitiveType The type of Gaussian primitives in the contracted
 *                        gaussian. Expected to be a specialization of
 *                        Primitive<T>.
 */
template<typename PrimitiveType>
class ContractedGaussianPIMPL {
public:
    /// Type *this is implementing
    using parent_type = ContractedGaussian<PrimitiveType>;

    /// Type the parent uses for sizes and indexing
    using size_type = typename parent_type::size_type;

    /// Traits class with all the types for a ContractedGaussian
    using cg_traits = ContractedGaussianTraits<parent_type>;

    /// Type used to store the coefficients
    using coefficient_vector =
      std::vector<typename cg_traits::coefficient_type>;

    /// Type used to store exponents
    using exponent_vector = std::vector<typename cg_traits::exponent_type>;

    // No primitives, centered at origin
    ContractedGaussianPIMPL() noexcept = default;

    // With provided primitives and center (no check cs.size() == exps.size())
    ContractedGaussianPIMPL(coefficient_vector cs, exponent_vector exps,
                            typename cg_traits::center_type r0) :
      m_center_(std::move(r0)),
      m_coefs_(std::move(cs)),
      m_exp_(std::move(exps)) {}

    // Adds a new primitive (assumed to be centered on `center()`)
    void add_primitive(typename cg_traits::coefficient_type c,
                       typename cg_traits::exponent_type e) {
        m_coefs_.reserve(m_coefs_.size() + 1);
        m_exp_.reserve(m_exp_.size() + 1);
        m_coefs_.push_back(c);
        m_exp_.push_back(e);
    }

    typename cg_traits::center_reference center() noexcept { return m_center_; }

    typename cg_traits::const_center_reference center() const noexcept {
        return m_center_;
    }

    typename cg_traits::coefficient_reference coefficient(
      size_type i) noexcept {
        return m_coefs_[i];
    }

    typename cg_traits::const_coefficient_reference coefficient(
      size_type i) const noexcept {
        return m_coefs_[i];
    }

    typename cg_traits::exponent_reference exponent(size_type i) noexcept {
        return m_exp_[i];
    }

    typename cg_traits::const_exponent_reference exponent(
      size_type i) const noexcept {
        return m_exp_[i];
    }

    auto operator[](size_type i) noexcept {
        using prim_ref = typename cg_traits::primitive_reference;
        return prim_ref(coefficient(i), exponent(i), center());
    }

    auto operator[](size_type i) const noexcept {
        using prim_ref = typename cg_traits::const_primitive_reference;
        return prim_ref(coefficient(i), exponent(i), center());
    }

    bool operator==(const ContractedGaussianPIMPL& rhs) const noexcept {
        auto lhs_state = std::tie(m_center_, m_coefs_, m_exp_);
        auto rhs_state = std::tie(rhs.m_center_, rhs.m_coefs_, rhs.m_exp_);
        return lhs_state == rhs_state;
    }

    /// The number of primitives in *this
    auto size() const noexcept { return m_coefs_.size(); }

private:
    /// Where the contracted Gaussian is centered
    typename cg_traits::center_type m_center_;

    /// The contraction coefficients
    coefficient_vector m_coefs_;

    /// The exponents
    exponent_vector m_exp_;
};

} // namespace chemist::basis_set::detail_
