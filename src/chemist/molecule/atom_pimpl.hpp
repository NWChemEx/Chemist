#pragma once
#include "chemist/detail_/aos_facade.hpp"
#include "chemist/molecule/atom.hpp"

namespace chemist::detail_ {

/**
 * @brief Class responsible for implementing the Atom class.
 *
 * There's two common views of a molecule class: A structure of arrays or an
 * array of structures.  In the former the properties of each atom are stored in
 * the molecule in a contiguous array, whereas in the latter the molecule stores
 * an array of Atom instances and each atom instance stores its own properties.
 * The goal of the PIMPL class is to isolate the user from these details.  In
 * particular there are two derived versions of AtomPIMPL: ContiguousAtomPIMPL
 * which is in charge of storing its own state and AliasAtomPIMPL which holds
 * pointers to the array stored within the Molecule.  Which of these is powering
 * the Atom class a user is using should not be a concern.
 */
class AtomPIMPL {
public:
    /// Type of a quantity for conveying how many
    using size_type = typename Atom::size_type;

    /// Type of the mass
    using mass_type = typename Atom::mass_type;

    /// Type of the coordinates
    using coord_type = typename Atom::coord_type;

    /// Type of the name
    using name_type = typename Atom::name_type;

    /// Does nothing because class has no state
    AtomPIMPL() = default;

    /// Does nothing because class has no state, overridden by derived class's
    virtual ~AtomPIMPL() = default;

    /**
     * @brief Returns a polymorphic copy of the derived class.
     *
     * This function returns a deep copy of the Atom.
     *
     * @return A copy of the derived class via a pointer to this class.
     *
     * @throw std::bad_alloc if there is insufficient memory to copy the class.
     *        Strong throw guarantee.
     */
    std::unique_ptr<AtomPIMPL> clone() const { return clone_(); }

    coord_type& coords() noexcept { return coords_(); }

    name_type& name() noexcept { return name_(); }

    size_type& at_num() noexcept { return at_num_(); }

    mass_type& mass() noexcept { return mass_(); }
    ///@}
protected:
    /**
     * @defgroup Copy/Move Functions
     *
     * As a polymorphic object copying/moving incorrectly can lead to problems.
     * We leave it up to the Atom class to implement everything besides deep
     * copy.  Deep copy can be done by invoking the clone method, which is
     * most naturally written in terms of the derived class's copy ctor, hence
     * the reason that the copy ctor of this class is protected (*i.e.* so it
     * can only be used by the derived class) while all others are deleted.
     *
     * @param[in] rhs The other instance to copy/move.
     *
     * @return The assignment version returns the current instance
     * after the copy/move has occurred.
     *
     * @throw None All move operations are no-throw guarantee.
     * @throw ??? Copy operations throw if the derived class throws.  Same
     *            guarantee as derived class.
     */
    ///@{
    AtomPIMPL(const AtomPIMPL& rhs) = default;

    AtomPIMPL(AtomPIMPL&& rhs) = delete;

    AtomPIMPL& operator=(const AtomPIMPL& rhs) = delete;

    AtomPIMPL& operator=(AtomPIMPL&& rhs) = delete;
    ///@}

private:
    /**
     * @brief Implemented by derived class to implement clone method
     *
     * Derived classes are responsible for ensuring that the returned AtomPIMPL
     * instance is a deep copy of the current instance.
     */
    virtual std::unique_ptr<AtomPIMPL> clone_() const = 0;

    /// Implemented by derived class to implement coords function
    virtual coord_type& coords_() noexcept = 0;

    /// Implemented by derived class to implement name()
    virtual std::string& name_() noexcept = 0;

    /// Implemented by derived class to implement at_num()
    virtual size_type& at_num_() noexcept = 0;

    /// Implemented by derived class to implement mass()
    virtual mass_type& mass_() noexcept = 0;
};

/// Implements PIMPL that assumes the atom's state is stored in a SoA
class ContiguousAtomPIMPL : public AtomPIMPL {
public:
    /// Type of an AoSElement holding an Atom's state
    using AoS_t = AoSElement<size_type, mass_type, coord_type>;
    /// Makes a massless atom, with an atomic number of 0, at the origin
    ContiguousAtomPIMPL() : my_name_(), impl_(std::make_shared<AoS_t>()) {
        impl_->insert<0>(0ul);
        impl_->insert<1>(0.0);
        impl_->insert<2>(coord_type{});
    }
    /// Wraps the PIMPL around an already existing AoSElement
    ContiguousAtomPIMPL(std::shared_ptr<AoS_t> impl) :
      my_name_(), impl_(impl) {}
    /// Default dtor
    ~ContiguousAtomPIMPL() override = default;

protected:
    /// Makes a deep copy of the PIMPL
    ContiguousAtomPIMPL(const ContiguousAtomPIMPL& rhs) :
      my_name_(rhs.my_name_), impl_(std::make_shared<AoS_t>(*rhs.impl_)) {}

private:
    /// Holds the name of the atom
    std::string my_name_;
    /// Holds the remainder of the Atom's state
    std::shared_ptr<AoS_t> impl_;

    /// Functions in this section implement the AtomPIMPL API
    ///@{
    std::unique_ptr<AtomPIMPL> clone_() const override {
        return std::unique_ptr<ContiguousAtomPIMPL>(
          new ContiguousAtomPIMPL(*this));
    }

    size_type& at_num_() noexcept override { return impl_->at<0>(); }

    mass_type& mass_() noexcept override { return impl_->at<1>(); }

    coord_type& coords_() noexcept override { return impl_->at<2>(); }

    name_type& name_() noexcept override { return my_name_; }
    ///@}
};

} // namespace chemist::detail_
