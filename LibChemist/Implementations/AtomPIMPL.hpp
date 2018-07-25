#pragma once
#include "LibChemist/Atom.hpp"
#include "LibChemist/Implementations/AoSFacade.hpp"

namespace LibChemist::detail_ {

/**
 * @brief Class responsible for implementing the Atom class.
 *
 * There's two common views of a molecule class: A structure of arrays or an
 * array of structures.  In the former the properties of each atom are stored in
 * the molecule in a contigous array, whereas in the latter the molecule stores
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

    mass_type& mass() noexcept {return mass_(); }
    ///@}
protected:
    /**
     * @defgroup Copy/Move Functions
     *
     * The copy and move functions for the AtomPIMPL class. The copy versions
     * are protected because the state of the `AtomPIMPL` class lives in the
     * derived class and thus slicing may occur if used from outside the
     * class.  Moving is deleted because it can not be done in a noexcept manner
     * from the derived class (base class may not be instances of the same
     * derived class).  Additionally, moving is not necessary as moving the
     * pimpl_ member of Atom is sufficient.  Copying rom outside the class
     * should be done via the `clone` member.
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

    AtomPIMPL& operator=(const AtomPIMPL& rhs) = default;

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

/// Implements PIMPL that allows Atom to store its own state.
class ContiguousAtomPIMPL : public AtomPIMPL {
    using AoS_t = AoSElement<size_type, mass_type, coord_type>;
public:
    ContiguousAtomPIMPL() : my_name_(), impl_(std::make_shared<AoS_t>()) {
        impl_->insert<0>(0ul);
        impl_->insert<1>(0.0);
        impl_->insert<2>(coord_type{});
    }

    ~ContiguousAtomPIMPL() override = default;

protected:
    ContiguousAtomPIMPL(const ContiguousAtomPIMPL& rhs) :
      my_name_(rhs.my_name_), impl_(std::make_shared<AoS_t>(*rhs.impl_)) {}
private:
    std::string my_name_;
    std::shared_ptr<AoS_t> impl_;
    
    std::unique_ptr<AtomPIMPL> clone_() const override {
        return
          std::unique_ptr<ContiguousAtomPIMPL>(new ContiguousAtomPIMPL(*this));
    }

    size_type& at_num_() noexcept override { return impl_->at<0>(); }

    mass_type& mass_() noexcept  override {return impl_->at<1>(); }

    coord_type& coords_() noexcept override { return impl_->at<2>(); }

    name_type& name_() noexcept override {return my_name_; }
};

} // namespace LibChemist::detail_
