#pragma once
#include <string>
#include <map>
#include <vector>
#include <istream>
#include "LibChemist/SetOfAtoms.hpp"

/** \file This file contains the machinery for parsing a string representation
 *  of a set of atoms.
 *
 * Unfortunately, like basis sets, there are a plethora of formats for
 * representing collections of atoms.  We again break the parsing into two
 * phases an examination of the line, and a data obtaining phase.  For the
 * former phase we have the following actions:
 * - none: the line is not worth parsing
 * - new_atom: the line contains information for a new atom
 * - same_atom: the line contains information for the current atom
 * - overall_system: the line contains information for the overall system
 *
 * If your parser signals the line is worth parsing you will be given the line
 * again and this time you will need to return the data the line contains. The
 * choices at the moment are:
 * - AtNum: the atomic number of the atom
 * - x: the x-coordinate of the atom (you are expected to ensure it is in a.u.
 * - y: same as x except for the y-coordinate
 * - z: same as x and y except for the z-coordinate
 * - charge: the overall system charge
 * - multiplicity: the overall system multiplicity
 *
 * \TODO The following data pieces will be needed eventually, but have not been
 * coded up:
 * - q: the charge of a point charge
 * - point: a dummy atom
 * - ghost: a ghost atom
 * - UCSides: the sides of the unit cell
 * - UCAngles: the angles of the unit cell
 * - SpaceGroup: which space group the unit cell belongs to.
 */


namespace LibChemist {


/** \brief This class abstracts away the layout of a string representation of a
 *  SetOfAtoms.
 *
 *  This is the base class for all classes specifying the layout of a
 *  SetOfAtoms file.
 */
struct SetOfAtomsFileParser
{
    enum class action_type{none,new_atom,same_atom,overall_system};
    enum class data_type{AtNum,x,y,z,charge,multiplicity};
    virtual action_type worth_parsing(const std::string& line)const=0;
    virtual std::map<data_type,std::vector<double>>
    parse(const std::string& line)const=0;
};

/** \brief This class implements a SetOfAtomsParser for the xyz format.
 *
 */
struct XYZParser: public SetOfAtomsFileParser
{
    action_type worth_parsing(const std::string& line)const override;
    std::map<data_type, std::vector<double> >
    parse(const std::string& line)const override;
};

/** \brief This class implements a SetOfAtomsParser for the xyz format,
 *         as described at https://en.wikipedia.org/wiki/XYZ_file_format
 *
 */
struct XYZWikiParser: public SetOfAtomsFileParser
{
    action_type worth_parsing(const std::string& line)const override;
    std::map<data_type, std::vector<double> >
    parse(const std::string& line)const override;
};


/** \brief The function to call to parse a SetOfAtomsFile.
 *
 * \param[in] is An input stream containing a string representation of a
 *               SetOfAtoms instance in a format the parser understands.
 * \param[in] parser The parser to be used to parse the input stream.
 * \returns The SetOfAtoms instance represented in the input stream.
 *
 */
SetOfAtoms parse_SetOfAtoms_file(std::istream& is,
                                 const SetOfAtomsFileParser& parser);

}//End namespace
