#pragma once

namespace chemist {

template<typename AtomicBSType>
struct AtomicBasisSetTraits : public AtomicBSType::shell_traits {
    /// String-like type used to store the basis set name
    using name_type = typename AtomicBSType::name_type;

    /// Mutable reference to the basis set's name
    using name_reference = typename AtomicBSType::name_reference;

    /// Read-only reference to the basis set's name
    using const_name_reference = typename AtomicBSType::const_name_reference;

    /// Unsigned integral type used to store the atomic number
    using atomic_number_type = typename AtomicBSType::atomic_number_type;

    /// Mutable reference to the basis set's atomic number
    using atomic_number_reference =
      typename AtomicBSType::atomic_number_reference;

    /// Read-only reference to the basis set's atomic number
    using const_atomic_number_reference =
      typename AtomicBSType::const_atomic_number_reference;

    /// Type used to return index ranges
    using range_type = typename AtomicBSType::range_type;

    /// Type of the Shells on this AtomicBasisSet
    using shell_type = typename AtomicBSType::value_type;

    /// Type of a read-/write-able reference to a AtomicBasisSet
    using shell_reference = typename AtomicBSType::reference;

    /// Type of a read-only reference to a AtomicBasisSet
    using const_shell_reference = typename AtomicBSType::const_reference;
};

} // namespace chemist
