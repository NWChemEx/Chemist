#pragma once

namespace chemist::wavefunction {

/** @brief Represents a wavefunction from a VectorSpace.
 *
 *  This class is at present a stub that we need so we can implement BraKet.
 *  It will be fleshed out in a forthcoming PR.
 */
class Wavefunction {
public:
    /// Type all wavefunctions inherit from
    using base_type = Wavefunction;

    /// Type acting like a mutable reference to a base_type object
    using base_reference = base_type&;

    /// Type acting like a read-only reference to a base_type object
    using const_base_reference = const base_type&;
};

} // namespace chemist::wavefunction