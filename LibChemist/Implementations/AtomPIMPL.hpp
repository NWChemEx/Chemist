#pragma once
#include "LibChemist/Atom.hpp"
#include <map>

namespace LibChemist::detail_ {

/**
 * @brief Class responsible for implementing the Atom class.
 *
 * There's two common views of a molecule class: A structure of arrays or an
 * array of structures.  In the former the properties of each atom are stored in
 * the molecule in a contigous array, whereas in the latter the molecule stores
 * an array of Atom instances and each atom instance stores its own properties.
 * The goal of the PIMPL class is to isolate the user from these details.  In
 * particular there are two derived versions of AtomPIMPL: StandAloneAtomPIMPL
 * which is in charge of storing its own state and AliasAtomPIMPL which holds
 * pointers to the array stored within the Molecule.  Which of these is powering
 * the Atom class a user is using should not be a concern.
 */
class AtomPIMPL {
public:
    /// Type of Atom's Property enum
    using Property = typename Atom::Property;

    /// Type of the object holding the property's value
    using property_type = typename Atom::property_type;

    /// Type of a quantity for conveying how many
    using size_type = typename Atom::size_type;

    /// Type of the coordinates
    using coord_type = typename Atom::coord_type;

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

    /**
     * @defgroup Atom Member Function Implementations
     *
     * @brief Functions that actually do the guts of the Atom's functions.
     *
     * The following functions are the public API to the PIMPL's virtual
     * methods.  Their documentation is the same as the corresponding Atom
     * member function.
     */
    ///@{
    size_type count(const Property& prop) const noexcept {
        return count_(prop);
    }

    coord_type& coords() noexcept { return coords_(); }

    property_type& property(const Property& prop) { return property_(prop); }

    std::string& name() noexcept { return name_(); }

    size_type& at_num() noexcept {return at_num_(); }

    void set_property(const Property& prop, property_type value) {
        set_property_(prop, value);
    }
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
    /// The name associated with this atom
    std::string da_name_;

    size_type Z_;

    /**
     * @brief Implemented by derived class to implement clone method
     *
     * Derived classes are responsible for ensuring that the returned AtomPIMPL
     * instance is a deep copy of the current instance.
     */
    virtual std::unique_ptr<AtomPIMPL> clone_() const = 0;

    /// Implemented by derived class to implement count method
    virtual size_type count_(const Property& prop) const noexcept = 0;

    /// Implemented by derived class to implement property function
    virtual property_type& property_(const Property& prop) = 0;

    /// Implemented by derived class to implement coords function
    virtual coord_type& coords_() noexcept = 0;

    /// Optionally can be overriden by derived class to implement name()
    virtual std::string& name_() noexcept { return da_name_; }

    /// Optionally can be overriden by derived class to implement at_num()
    virtual size_type& at_num_() noexcept {return Z_; }

    /// Implemented by derived class to implement setting of a property
    virtual void set_property_(const Property& prop, property_type value) = 0;
};

/// Implements PIMPL that allows Atom to store its own state.
class StandAloneAtomPIMPL : public AtomPIMPL {
    using prop_map_type = typename std::map<Property, property_type>;
public:
    StandAloneAtomPIMPL() = default;
    ~StandAloneAtomPIMPL() override = default;

protected:
    StandAloneAtomPIMPL(const StandAloneAtomPIMPL& rhs) = default;
    StandAloneAtomPIMPL& operator=(const StandAloneAtomPIMPL&) = default;

private:
    prop_map_type props_;

    coord_type da_coords_ = {};

    std::unique_ptr<AtomPIMPL> clone_() const override {
        return std::unique_ptr<AtomPIMPL>(new StandAloneAtomPIMPL(*this));
    }

    size_type count_(const Property& prop) const noexcept override {
        return props_.count(prop);
    }

    property_type& property_(const Property& prop) override {
        return props_.at(prop);
    }

    coord_type& coords_() noexcept override { return da_coords_; }

    void set_property_(const Property& prop, property_type value) override {
        props_[prop] = value;
    };
};

} // namespace LibChemist::detail_
