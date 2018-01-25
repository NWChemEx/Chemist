#pragma once
#include "LibChemist/BasisShell.hpp"
#include <istream>
#include <map>
#include <string>
#include <vector>

/** \file This file contains the machinery for parsing a basis set file.
 *
 * Unfortunately there are a ton of different basis set file types.  At the
 * end of the day they all look something like:
 *
 * \verbatim
   Header (comments, and other information)

   Start of an Atom
   Shell 1
   Shell 2
   ...
   Start of Next Atom
   Shell 1
   Shell 2
   ...

   Footer
   \endverbatim
 *
 * Here's how this is going to work.  I'm going to loop over a file and give
 * you a line.  You're going to tell me if that line is worth parsing by
 * returning one of the atom_type enums: none, same_atom, new_atom.
 * Respectively, these signal the line is not worth parsing, it is worth
 * parsing and the resulting data is for the current atom, it is worth parsing
 * but the data is for a new atom. (Note the first occurrence of parsable data
 * should be signaled by new_atom).
 *
 * For every parsable line you return an std::map<data_type,double> of the
 * data you obtained from the line.
 */

namespace LibChemist {

/** \brief This class abstracts away the layout of the basis set file
 *
 *  This is the base class for all classes specifying the layout of a
 *  basis set file.
 */
struct BasisSetFileParser {
    enum class action_type
    {
        none,
        new_atom,
        new_shell,
        same_shell
    };
    enum class data_type
    {
        exponent,
        coefficient,
        angular_momentum,
        Z
    };

    virtual action_type worth_parsing(const std::string& line) const = 0;
    virtual std::map<data_type, std::vector<double>> parse(
        const std::string& line) const = 0;
};

/** \brief This class implements a BasisSetFileParser for the Gaussian94 format.
 *
 */
struct G94 : public BasisSetFileParser {
    action_type worth_parsing(const std::string& line) const override;
    std::map<data_type, std::vector<double>> parse(
        const std::string& line) const override;
};

/** \brief The function to call to parse a BasisSetFile.
 *
 *
 */
std::map<size_t, std::vector<BasisShell>> parse_basis_set_file(
    std::istream& is,
    const BasisSetFileParser& parser);

} // End namespace LibChemist
