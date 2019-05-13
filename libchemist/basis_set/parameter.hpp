#pragma once
#include <optional>

namespace libchemist::basis_set {

/** @brief A simple wrapper class for holding a parameter associated with the
 *         basis set.
 *
 *  Ultimately our basis set is going to hold a bunch of parameters. Users are
 *  then going to interact with those parameters via views. We need to support
 *  two cases: when the Parameter instance owns the value and when we are only
 *  aliasing the value. The base class defines the API for interacting with the
 *  value, and holds the actual parameter when the instance owns the value. The
 *  derived class `ParameterAlias` is used to interact with the value as an
 *  alias.
 *
 *  @note This class is capable of being used purely at compile-time by making
 *        a constexpr instance.
 *
 * @tparam ElementType The type of the value we are wrapping.
 */
template<typename ElementType>
class Parameter {
public:
    /// Type of this object
    using my_type = Parameter<ElementType>;

    /// Type of a reference to the wrapped value
    using reference = ElementType&;

    /// Type of a read-only reference to the wrapped value
    using const_reference = const ElementType&;

    /** @brief Creates an instance that owns its value
     *
     *  @param value The value to set the owned value to.
     */
    constexpr Parameter(ElementType value) noexcept;
    constexpr Parameter(const my_type& rhs) noexcept;
    constexpr my_type& operator        =(const my_type& rhs) noexcept;
    constexpr Parameter(my_type&& rhs) = default;
    constexpr my_type& operator=(my_type&& rhs) = default;
    ~Parameter()                                = default;

    /// Returns the value of the parameter
    constexpr reference value() { return m_value_.value(); }
    /// Returns a read-only version of the parameter
    constexpr const_reference value() const { return *m_pvalue_; }

protected:
    constexpr Parameter(ElementType* pvalue) noexcept : m_pvalue_(pvalue) {}

private:
    std::opational<ElementType> m_value_;
    // std::reference_wrapper<ElementType> m_pvalue_;
};

template<typename ElementType>
constexpr Parameter<ElementType>::Parameter(ElementType value) noexcept :
  m_value_(std::move(value)),
  m_pvalue_(m_value_) {}

template<typename ElementType>
constexpr Parameter<ElementType>::Parameter(
  const Parameter<ElementType>& rhs) noexcept :
  Parameter(rhs.value()) {}

template<typename ElementType>
constexpr Parameter<ElementType>& Parameter<ElementType>::operator=(
  const Parameter<ElementType>& rhs) noexcept {
    return *this = std::move(Parameter<ElementType>(rhs));
}

///** @brief
// *
// */
// template<typename ElementType>
// class ParameterAlias : public Parameter<ElementType> {
// public:
//    using my_type   = ParameterAlias<ElementType>;
//    using base_type = Parameter<ElementType>;
//    using reference = ElementType&;
//    using const_reference = const ElementType&;
//    constexpr ParameterAlias(ElementType* pvalue) : base_type(pvalue) {}
//    constexpr ParameterAlias(const base_type& rhs) : base_type(&rhs.value())
//    {}
//};
//
// template<typename ElementType>
// constexpr ParameterAlias<ElementType>&
// ParameterAlias<ElementType>::operator=(const base_type& rhs) {
//    return *this = std::move(ParameterAlias(&rhs.value()));
//}

} // namespace libchemist::basis_set
