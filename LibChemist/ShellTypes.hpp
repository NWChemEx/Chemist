#pragma once
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace LibChemist {

/** \brief Type of basis shells */
enum class ShellType { SphericalGaussian, CartesianGaussian, Slater };

/** \brief Returns a string corresponding to ShellType.
 *
 *  This function basically exists so that you can print out enumerations.
 *
 *  \param[in] type The ShellType you want the string representation of.
 *  \returns The string representation of \p type.
 *  \throws std::logic_error if \p type is not a recognized ShellType.
 *  \todo Verify if it is even possible to trip the throw
 *
 */
inline const char* shelltype_string(ShellType type) {
    switch(type) {
        case ShellType::SphericalGaussian: return "SphericalGaussian";
        case ShellType::CartesianGaussian: return "CartesianGaussian";
        case ShellType::Slater: return "Slater";
        default: throw std::logic_error("Shell type to string is missing");
    }
}

/** \brief Function for converting an angular momentum's string identifier to
 *  an integer.
 *
 * \param[in] am The letter (or letters for combined shells) of the angular
 *                momentum.  \p am must be in the range s,p,d,f,g...z or in
 *                the range s,sp,spd,...,spdfgh.
 * \returns The integer corresponding to \p am.
 * \throw std::out_of_range if \p am is not one of the recognized string
 *    represenations of angular momentum.  Strong throw guarantee.
 */
int am_str2int(const std::string& am);

/** \brief Function for converting an angular momentum's integer identifier to
 *  a string.
 * \param[in] am The integer to get the corresponding angular momentum for.  \p
 *               am must be in the range [-5,21].
 * \returns The string for \p am.
 * \throw std::out_of_range if \p am is not in the range[-5,21].  Strong throw
 *   guarantee.
 */
std::string am_int2str(int am);

/** \brief Returns the i-th angular momentum in a general contraction.
 *
 *  \param[in] am The angular momentum value returned by am_str2int or expected
 *                by am_int2str.
 *  \param[in] i  Which angular momentum of the general contraction to return.
 *
 *  \throws std::bad_alloc if memory allocation fails.  Strong throw guarantee.
 */
size_t am_2int(int am, size_t i);

} // namespace LibChemist
