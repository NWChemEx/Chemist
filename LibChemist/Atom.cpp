#include "LibChemist/Atom.hpp"
#include <tuple>
#include "LibChemist/Implementations/AtomPIMPL.hpp"

namespace LibChemist {

using Property = Atom::Property;
using property_type = double;
using size_type = Atom::size_type;
using coord_type = Atom::coord_type;
using prop_map_type = std::map<Property, property_type>;

  namespace detail_ {
    /// Implements PIMPL that allows Atom to store its own state.
    class StandAloneAtomPIMPL : public AtomPIMPL {
    public:
        StandAloneAtomPIMPL()  = default;
        StandAloneAtomPIMPL(prop_map_type&& map): props_(std::move(map)){}

        ~StandAloneAtomPIMPL() override = default;

    protected:
        StandAloneAtomPIMPL(const StandAloneAtomPIMPL& rhs)        = default;
        StandAloneAtomPIMPL& operator=(const StandAloneAtomPIMPL&) = default;
    private:
        prop_map_type props_;

        coord_type da_coords_={};

        std::unique_ptr<AtomPIMPL> clone_()const override {
            return std::unique_ptr<AtomPIMPL>(new StandAloneAtomPIMPL(*this));
        }

        size_type count_(const Property& prop)const noexcept override {
            return props_.count(prop);
        }

        property_type& property_(const Property& prop) override {
            return props_.at(prop);
        }

        coord_type& coords_() noexcept override { return da_coords_; }


    };
}

Atom::Atom() : pimpl_(std::make_unique<detail_::StandAloneAtomPIMPL>()) {}
Atom::Atom(const Atom& rhs) : pimpl_(rhs.pimpl_->clone()) {}
Atom::Atom(Atom&& rhs) noexcept : pimpl_(std::move(rhs.pimpl_)) {}
Atom& Atom::operator=(Atom rhs) noexcept {
    pimpl_.swap(rhs.pimpl_);
    return *this;
}
Atom& Atom::operator=(Atom&& rhs) noexcept {
    pimpl_.swap(rhs.pimpl_);
    return *this;
}
Atom::Atom(prop_map_type&& props) :
  pimpl_(std::make_unique<detail_::StandAloneAtomPIMPL>(std::move(props))){}

Atom::~Atom(){}

size_type Atom::count(const Property& prop)const noexcept{
    return pimpl_->count(prop);
}

coord_type& Atom::coords() noexcept {
    return pimpl_->coords();
}

property_type& Atom::property(const Property& prop) {
    return pimpl_->property(prop);
}

} // namespace LibChemist
