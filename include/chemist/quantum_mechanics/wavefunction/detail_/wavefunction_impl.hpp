#pragma once
#include <chemist/quantum_mechanics/wavefunction/wavefunction_class.hpp>

namespace chemist::wavefunction::detail_ {

/** @brief Code factorization for implementing Wavefunction's virtual methods.
 *
 *  @tparam DerivedType The type of the derived class *this is implementing.
 *
 *  A number of the virtual functions of the Wavefunction class can be
 *  implemented by making some basic assumptions about the derived class
 *  including:
 *   - Defines a copy ctor
 *   - is value comparable
 *  and by knowing the type of the derived class. The Wavefunction Impl class
 *  is code factorization for implementing the virtual functions which fall
 *  into this category.
 */
template<typename DerivedType>
class WavefunctionImpl : public Wavefunction {
protected:
    /// Implements clone() by calling DerivedType's copy ctor
    base_pointer clone_() const override {
        return std::make_unique<DerivedType>(downcast_());
    }

    /// Implements are_equal by calling DerivedType's operator==
    bool are_equal_(const_base_reference rhs) const noexcept override {
        const auto& lhs = downcast_();
        auto prhs       = dynamic_cast<const DerivedType*>(&rhs);
        if(prhs == nullptr) return false;
        return lhs == (*prhs);
    }

private:
    /// Wraps downcasting *this to DerivedType&
    DerivedType& downcast_() { return static_cast<DerivedType&>(*this); }

    /// Wraps downcast *this to const DerivedType&
    const DerivedType& downcast_() const {
        return static_cast<const DerivedType&>(*this);
    }
};

} // namespace chemist::wavefunction::detail_