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

.. _architecture_of_the_quantum_mechanics_component:

####################################################
Architecture of the Quantum Mechanics (QM) Component
####################################################

The need for the QM component stemmed from the high-level
architecture details described :ref:`architecture_of_chemist`.

*************************
What is the QM Component?
*************************

The chemical system component of Chemist is used to describe the chemistry the
user wants to do. To compute the quantities of interest the chemical system
must be mapped to quantum mechanical equations. The QM component is charged with
describing the resulting equations.

********************************
Why do we need the QM component?
********************************

The QM component constitutes a :term:`DSL` for specifying tensors and tensor
elements using a bra-ket like syntax. This is the key to being able to specify
electronic structure methods using programmatic abstractions that behave like
the physics they implement.  For example, want to do DFT vs SCF? Add an operator
for the exchange-correlation potential. Want to freeze the core electrons in
the calculation of the correlation energy? Simply delete the terms involving
core excitations. Want to do MP2-F12 vs traditional MP2? Add a term to the
wavefunction that is linear in the electron-electron separation. Ultimately,
there are a ton of potential methods which stem from combinations of
enabling/disabling operators and wavefunction choices. A DSL allows us to
express them all, whether or not we have names for the approximation.

*****************
QM Considerations
*****************

.. _qm_dirac_notation:

Dirac notation
   The "natural" way to write QM equations is in Dirac notation. The QM
   component should start in Dirac notation and end in tensor equations

.. _qm_tensor_equations:

tensor equations
   Modern scientific computing is tensor-based. The resulting tensor equations
   should specify the tensor operations needed to compute the property of
   interest and know where to get the input values from.

.. _qm_operators:

operators
   To write Dirac notation we require two things: operators and wavefunctions.
   The operators describe the interactions/property to compute.

.. _qm_wavefunctions:

wavefunctions
   To write Dirac notation we require two things: operators and wavefunctions.
   The wavefunctions describe the QM state of the system.

*****************
Example Use Cases
*****************

The following pseudocode illustrates how pieces of the QM component are used to
"calculate" quantities. This is an aspirational code snippet and may not
reflect the final DSL.

.. code-block:: c++

   Hamiltonian<ManyElectron, Nuclei> H = make_molecular_hamiltonian();
   RDeterminant Psi                    = get_determinant();

   // actually declares a BraKet instance, E, relying on C++17's CTAD to work
   // out the template type parameters.
   auto E = BraKet(Psi, H, Psi);

   // Again E_elec is a BraKet instance and not the actual energy
   auto E_elec = BraKet(Psi, H.electronic(), Psi);

   // Use other operators to get other terms
   Kinetic<ManyElectron> T_e = get_many_electron_kinetic_energy_operator();
   auto E_kinetic = BraKet(Psi, T_e, Psi);

   CMOs cmos             = get_orbitals(); // Get the orbitals used to build Psi
   Kinetic<Electron> t_e = get_one_electron_kinetic_energy_operator();
   auto t_matrix         = BraKet(cmos, t_e, cmos);


**************************
Design of the QM Component
**************************

TODO: BraKet class for Dirac notation
TODO: SeQuant connection for tensor equations.

Operators
=========

Main article: :ref:`architecture_of_operator`.

Wavefunctions
=============

Main article: :ref:`architecture_of_wavefunction`.


**********
QM Summary
**********

:ref:`qm_dirac_notation`
   We have...

:ref:`qm_tensor_equations`
   We have...

:ref:`qm_operators`
   We have created the operator subcomponent to address this consideration.

:ref:`qm_wavefunctions`
   We have created the wavefunction subcomponent to address this consideration.
