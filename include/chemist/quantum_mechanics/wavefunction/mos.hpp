#pragma once
#include <chemist/quantum_mechanics/wavefunction/aos.hpp>
#include <chemist/quantum_mechanics/wavefunction/transformed.hpp>

namespace chemist::wavefunction {

/** @brief Space spanned by the molecular orbitals (MOs).
 *
 *  This is a strong type to represent that a particular set of transformed
 *  AOs are MOs.
 */
class MOs : public Transformed<AOs> {
private:
    /// Type of the base class
    using transformed_type = Transformed<AOs>;

public:
    /// Pull in types from the base class
    ///@{
    using typename transformed_type::base_pointer;
    using typename transformed_type::size_type;
    ///@}

    /// Reuse base class's ctor
    using transformed_type::transformed_type;

protected:
    /// Implements clone by calling copy ctor
    base_pointer clone_() const override {
        return std::make_unique<MOs>(*this);
    }

    /// Implements are_equal by calling are_equal_impl_
    bool are_equal_(const VectorSpace& rhs) const noexcept override {
        return are_equal_impl_<MOs>(rhs);
    }
};

} // namespace chemist::wavefunction