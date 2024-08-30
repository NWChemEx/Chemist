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

The need for the Wavefunction component stemmed from the high-level
architecture details described :ref:`architecture_of_chemist`.

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

Together the Operator and Wavefunction components constitute a :term:`DSL` for
specifying tensors and tensor elements using a bra-ket like syntax. This is the
key to being able to specify electronic structure methods using programmatic
abstractions that behave like the physics they implement.  For example, want to
do DFT vs SCF? Add an operator for the exchange-correlation potential. Want to
freeze the core electrons in the calculation of the correlation energy? Simply
delete the terms. Want to do MP2-F12 vs traditional MP2? Add a term to the
wavefunction that is linear in the electron-electron separation. Ultimately,
there are a ton of potential methods which stem from combinations of
enabling/disabling operators and wavefunction choices. A DSL allows us to
express them all, whether or not we have names for the approximation.

***************************
Wavefunction Considerations
***************************

orbitals
  Orbitals are the building blocks of many-particle wavefunctions. We need to
  be able to represent the set of orbitals from which the many-particle
  wavefunction is built.

implicit vs explicit
   Some basis functions are typically explicit (e.g., atomic orbitals,
   molecular orbitals) meaning they have state which must be explicitly
   specified. Other basis functions are considered implicit (e.g., spin,
   Cartesian axis).



*****************
Example Use Cases
*****************

.. code-block:: c++

   // --------------------------------------------------------------------------
   // --- Examples of building vector spaces
   // --------------------------------------------------------------------------
   AOSpace aos(get_ao_parameters());
   Spin spins(std::ratio(1, 2)); // Takes total spin of particle

   // Spatial molecular orbitals (MOs) are formed by transforming AOs.
   using MOSpace = Transformed<AOSpace>;

   // Spin (molecular) orbitals (SOs) are products of an MO and a spin function
   using SOSpace = Product<MOSpace, Spin>;

   // For Transformed<T> input is the transformation and the "from" space
   MOSpace mos(c, aos);
   MOSpace mo_alpha(c_alpha, aos);
   MOSpace mo_beta(c_beta, aos);


   SOSpace sos(mo_alpha * spins[0], mo_beta * spins[1]);

   using CanonicalMOSpace = Natural<MOSpace>;
   using CanonicalSOSpace = Natural<SOSpace>;
   CanonicalMOSpace cmos(orbital_energies, mos);
   CanonicalSOSpace csos(spin_orbital_energies, sos);

   // Create a determinant space for n particles
   FockSpace<CanonicalMOSpace> fock_n(n, cmos); // Occupies first n orbitals
   auto d0 = fock_n.vacuum(); //Get the vacuum state

   fock_n.set_vacuum(/* list of occupied orbitals */);
