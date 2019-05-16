#pragma once
#include "libchemist/types.hpp"
#include <tuple>

namespace libchemist::basis_set::detail_ {

template<typename CenterType, typename ExpContainer, typename CoefContainer,
         typename PrimContainer>
struct GaussianAOTraits {
    using center_type    = CenterType;
    using exp_container  = ExpContainer;
    using coef_container = CoefContainer;
    using prim_container = PrimContainer;
};

/** @brief Consolidates the types relevant for an AOBasisSet into a single class
 *
 *  The AOBasisSet and related classes rely heavily on a series of types to
 *  determine what properties are read/write-able and which are read-only.
 *  This class collects all of those types into one class so that we only need
 *  to pass it around. For the most part these types should be POD types or
 *  pointers to POD types that are possibly const-qualified.
 *
 *
 * @tparam T          double or float, used to set the defaults for the other
 *                    types.
 * @tparam ExpType    The type of a Gaussian's exponent, defaults to @p T
 * @tparam CoefType   The type of a Gaussian's contraction coefficient,
 *                    defaults to @p T
 * @tparam CenterType Type used to hold the Cartesian coordinates of the point
 *                    the basis function, shell, etc. is centered on. Defaults
 *                    type::coord<T>.
 * @tparam AMType     type used to hold the angular momentum defaults to
 *                    type::angular_momentum
 * @tparam PureType   type used to hold the flag that dictates whether or not
 *                    the basis function is Cartesian or spherical (pure).
 *                    Defaults to bool
 */
template<typename T, typename ExpType = T, typename CoefType = T,
         typename CenterType = type::coord<T>,
         typename AMType = type::angular_momentum, typename PureType = bool>
struct BasisSetTraits {
    /// Type used to store the Gaussian's center
    using center_type = CenterType;
    /// Type used to store the magnitude of the angular momentum
    using am_type = AMType;
    /// Type used to store whether the basis function is Cartesian or spherical
    using pure_type = PureType;
    /// Type used to hold the Gaussian's exponent
    using exp_type = ExpType;
    /// Type used to hold the Gaussian's coefficient
    using coef_type = CoefType;
};

/** @brief We store the basis set parameters either by value or by a pointer,
 *         this function dereferences the pointer if need be.
 *
 * This function uses template meta-programming to determine if the input is a
 * pointer or a value. If the input is a pointer, it dereferences the pointer
 * and returns the result, otherwise it just returns the input. This function
 * is used internally to provide a consistent API throughout the ``Gaussian``,
 * ``GaussianAO``, ``AOShell``, and ``AOCenter`` classes regardless of whether
 * the class holds the value or a handle to the value.
 *
 * @tparam T the type used to store the value, will be cv-qualified and may be
 *           a pointer.
 * @param[in] value The value we are possibly dereferencing.
 * @return The object pointed at, if @p value is a pointer, otherwise @p value.
 * @throw none No throw guarantee.
 */
template<typename T>
constexpr auto& get_value(T&& value) noexcept {
    constexpr bool is_ptr = std::is_pointer_v<std::decay_t<T>>;
    if constexpr(is_ptr)
        return *value;
    else
        return value;
}

/** @brief Type traits for a Gaussian when it's part of a ``GaussianAO``
 *
 *  We typically think of the ``GaussianAO`` as factoring out the coordinates
 *  of each primitive ``Gaussian``
 */
template<typename T>
using AOFxnPrim = BasisSetTraits<T,
                                 T&, // exponent
                                 T&, // coef
                                 const type::coord<std::decay_t<T>>&, // center
                                 const type::size, // angular mom
                                 const bool>;      // purity

/// Type traits of a basis function in a typical AOShell
template<typename T>
using AOShellFxn = BasisSetTraits<T,
                                  const type::coord<T>*, // center
                                  const type::size,      // angular mom
                                  const bool,            // purity
                                  const T*,              // exponent
                                  const T*>;             // coef

/// Type traits of a shell in a typical AOCenter
template<typename T>
using AOCenterShell = BasisSetTraits<T,
                                     const type::coord<T>*, // center
                                     type::size,            // angular mom
                                     bool,                  // purity
                                     const T*,              // exponent
                                     const T*>;             // coef

} // namespace libchemist::basis_set::detail_
