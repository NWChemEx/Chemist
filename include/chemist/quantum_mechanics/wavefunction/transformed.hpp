#pragma once
#include <chemist/quantum_mechanics/wavefunction/vector_space.hpp>

namespace chemist::wavefunction {

template<typename FromSpaceType>
class Transformed : public VectorSpace {
private:
    using my_type = Transformed<FromSpaceType>;

public:
    using VectorSpace::base_pointer;
    using VectorSpace::size_type;

    using transform_type = tensorwrapper::Tensor;

    using transform_reference = transform_type&;

    using const_transform_reference = const transform_type&;

    using from_space_type = FromSpaceType;

    using from_space_reference = from_space_type&;

    using const_from_space_reference = const from_space_type&;

    Transformed(from_space_type from_space, transform_type c) :
      m_space_(std::move(from_space)), m_c_(std::move(c)) {}

    from_space_reference from_space() { return m_space_; }

    const_from_space_reference from_space() const { return m_space_; }

    transform_reference transform() { return m_c_; }

    const_transform_reference transform() const { return m_c_; }

    bool operator==(const Transformed& rhs) const noexcept {
        return std::tie(m_space_, m_c_) == std::tie(rhs.m_space_, rhs.m_c_);
    }

    bool operator!=(const Transformed& rhs) const noexcept {
        return !((*this) == rhs);
    }

protected:
    /// Implements clone by calling copy ctor
    base_pointer clone_() const override {
        return std::make_unique<my_type>(*this);
    }

    /// Implements size by returning number of columns
    size_type size_() const noexcept override {
        // XXX: Assume square matrix
        return m_space_.size();

        // return m_c_.logical_layout().shape().extent(1);
    }

    /// Implements are_equal by calling are_equal_impl_
    bool are_equal_(const VectorSpace& rhs) const noexcept override {
        return are_equal_impl_<my_type>(rhs);
    }

private:
    from_space_type m_space_;

    transform_type m_c_;
};

} // namespace chemist::wavefunction