.. Copyright 2024 NWChemEx-Project
..
.. Licensed under the Apache License, Version 2.0 (the "License");
.. you may not use this file except in compliance with the License.
.. You may obtain a copy of the License at
..
.. http://www.apache.org/licenses/LICENSE-2.0
..
.. Unless required by applicable law or agreed to in writing, software
.. distributed under the License is distributed on an "AS IS" BASIS,
.. WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
.. See the License for the specific language governing permissions and
.. limitations under the License.

.. _architecture_of_wavefunction:

######################################
Architecture of Wavefunction Component
######################################

The need for a wavefunction component was motivated in
:ref:`architecture_of_the_quantum_mechanics_component`.

***********************************
What is the Wavefunction Component?
***********************************

In the traditional view of quantum chemistry, the wavefunction represents the
(possibly time-dependent) state of the system. Properties of the system are
computed as tensor elements involving wavefunctions and operators. The
Wavefunction component contains classes which facilitate the creation and
manipulation of a chemical system's wavefunction.

****************************************
Why do we need a Wavefunction Component?
****************************************



***************************
Wavefunction Considerations
***************************

orbitals
   Orbitals are the building blocks of many-particle wavefunctions. We need to
   be able to represent the set of orbitals from which the many-particle
   wavefunction is built.

determinants and permanents
   Determinants/permanents are the many-particle wavefunctions with the right
   spin symmetry for fermions/bosons respectively.

basis function vs basis set
   While the intermediates used to compute a property are often expressed as
   tensors, the property we want is often a single element of a larger tensor.
   We thus need a way to specify "compute the entire tensor representation" vs
   "compute the 0,0-th element".

implicit vs explicit
   Some basis functions are typically explicit (e.g., atomic orbitals,
   molecular orbitals) meaning they have state which must be explicitly
   specified. Other basis functions are considered implicit (e.g., spin,
   Cartesian axis).

method dispatching
   One use of the wavefunction is to dispatch to different methods. For example,
   having different types for restricted vs unrestricted self-consistent field
   theory wavefunctions makes it easier to dispatch to RSCF/USCF respectively.

bra/ket state
   Aside from indicating the computation to do, the wavefunction will also be
   used to pass the parameters needed to describe the state of the bra or ket.
   Examples include:

      - AO basis set parameters.
      - molecular orbital coefficients
      - orbital occupations


*****************
Example Use Cases
*****************

The following pseudocode snippet is designed to show some high level examples of
using the wavefunction component. It is not meant to be true to the final
Chemist library (e.g., many of the typedefs exist as strong types for user
ease), but simply motivational.

.. code-block:: c++

   AOSpace aos(get_ao_parameters());
   Spin spins(std::ratio(1, 2)); // Takes total spin of particle

   // Can "slice" spaces to make subspaces
   Spin alpha = spins.subspace(0, 1); // Spins count down
   Spin beta  = spins.subspace(1, 2);

   // Spatial molecular orbitals (MOs) are formed by transforming AOs.
   using MOSpace = Transformed<AOSpace>;

   // Input is the transformation and the "from" space
   MOSpace mos(c, aos);

   // Spin atomic orbitals (SAOs) are products of AOs and spin
   using SAOSpace = Product<AOSpace, Spin>;

   // Input are the spaces to take the tensor product of
   SAOSpace saos_alpha(aos, alpha);
   SAOSpace saos_beta(aos, beta);
   SAOSpace saos(aos, spins); //Union of saos_alpha and saos_beta

   // Could have also obtained saos_alpha/saos_beta by slicing saos
   // Could have also obtained saos by taking union of saos_alpha and saos_beta

   // Transforming SAOs in general leads to spinors. Spin (molecular) orbitals
   // (SOs) arise when coefficients can't mix spins
   using SOSpace = Transformed<SAOSpace>;

   SOSpace spinors(c_spinors, saos);
   SOSpace so_alpha(c_alpha, saos_alpha);
   SOSpace so_beta(c_beta, saos_beta);
   SOSpace sos = so_alpha + so_beta; // Can treat the SMOs as a unified space

   // Natural spaces diagonalize a quantity. Canonical means they diagonalize
   // the Fock matrix
   using CanonicalMOSpace = Natural<MOSpace>;
   using CanonicalSOSpace = Natural<SOSpace>;

   // Input is the diagonal elements and the orbitals
   CanonicalMOSpace cmos(orbital_energies, mos);
   CanonicalSOSpace csos(spin_orbital_energies, sos);

   // For an n-particle system, orbital products of n orbitals are then
   // antisymmetrized/symmetrized (for fermions/bosons respectively) to form
   // n-particle basis functions
   using RestrictedDeterminants = Antisymmetrized<CanonicalMOSpace>;
   using UnrestrictedDeterminants = Antisymmetrized<CanonicalSOSpace>;
   RestrictedDeterminants rdeterminant(n, cmos);
   UnrestrictedDeterminants udeterminant(n, csos);

   // By default the first n orbitals are used to form the reference state
   // Can change the reference. This call for example sets the reference to
   // orbitals 1 through n inclusive.
   udeterminant.set_reference(1, 2, 3, ..., n);

   // It's common to specify determinants based on number of excitations. This
   // slices the determinant space to limit it to double excitations
   auto doubles = rdeterminant.excitation_subspace<2>();
