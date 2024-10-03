#pragma once
#include <chemist/quantum_mechanics/operator/operator_base.hpp>
#include <memory>

namespace chemist::braket::detail_ {

/** @brief Code factorization for APIs and implementations common to all bra-
 *         kets.
 *
 *  BraKet objects come in two flavors, those that derive from TensorElement
 *  and those that derive from TensorRepresentation. The resulting BraKet
 *  operates largely the same regardless of which class it derives from. To
 *  factor out the common APIs and implementations we make both TensorElement
 *  and TensorRepresentation derive from BraKetBase via CRTP (needed so this
 *  class can get the types right for the API).
 *
 *  @tparam DerivedType The class *this is defining the API for. Assumed to be
 *                      TensorElement or TensorRepresentation.
 */
template<typename DerivedType>
class BraKetBase {
public:
    /// Type of the base class (as far as users are concerned)
    using base_type = DerivedType;

    /// Type of a pointer to a BraKet object passed as a base_type object
    using base_pointer = std::unique_ptr<base_type>;

    /// Type of a mutable reference to an object of type base_type
    using base_reference = base_type&;

    /// Type of a read-only reference to an object of type base_type
    using const_base_reference = const base_type&;

    /// Type all operator objects derive from
    using operator_base_type = qm_operator::OperatorBase;

    /// Type behaving like a reference to a mutable operator_base_type
    using operator_base_reference = typename operator_base_type::base_reference;

    /// Type behaving like a reference to a read-only operator_base_type
    using const_operator_base_reference =
      typename operator_base_type::const_base_reference;

    /// No throw polymorphic dtor
    virtual ~BraKetBase() noexcept = default;

    base_pointer clone() const { return clone_(); }

    bool are_equal(const_base_reference rhs) const noexcept {
        return are_equal_(rhs) && rhs.are_equal_(*this);
    }

protected:
    using bra_ket_base_type                      = BraKetBase<DerivedType>;
    BraKetBase() noexcept                        = default;
    BraKetBase(const BraKetBase&)                = default;
    BraKetBase(BraKetBase&&) noexcept            = default;
    BraKetBase& operator=(const BraKetBase&)     = default;
    BraKetBase& operator=(BraKetBase&&) noexcept = default;

    /// Override to be consistent with clone description
    virtual base_pointer clone_() const = 0;

    /// Override to be consistent with are_equal description
    virtual bool are_equal_(const_base_reference rhs) const noexcept = 0;
};

} // namespace chemist::braket::detail_