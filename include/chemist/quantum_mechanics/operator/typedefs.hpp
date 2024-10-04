#pragma once
#include <chemist/chemical_system/electron/electron.hpp>
#include <chemist/chemical_system/nucleus/nucleus.hpp>
#include <chemist/quantum_mechanics/operator/kinetic.hpp>

/** @file typedefs.hpp
 *
 *  To make the user's life easier we have typedefed common template
 *  instantiations of various operators. Types are derived using the following
 *  commonly used conventions:
 *
 *  - all typedefs end with "_type" as we suspect the user may name their
 *    variable using similar conventions.
 *  - uppercase letter denotes a many-particle operator (or more generally an
 *    operator with an indefinite number of particles)
 *  - lowercase letter denotes a one-particle operator (or more generally an
 *    operator with a definite number of particles)
 *  - after the letter for the operator we denote the particles involved using
 *    the convention that e = electron(s) and n = nucleus or nuclei
 *  - t/T is kinetic energy operator
 *
 *  @note Right now this file includes the headers needed to instantiate
 *        instances of these typedefs; however, it is possible to declare these
 *        typedefs without including those files (this requires us writing
 *        forward declarations for each class). If that happens this file should
 *        probably use the forward declarations instead...
 */

namespace chemist::qm_operator {

using t_e_type = Kinetic<Electron>;
using T_e_type = Kinetic<ManyElectrons>;
using t_n_type = Kinetic<Nucleus>;
using T_n_type = Kinetic<Nuclei>;

} // namespace chemist::qm_operator