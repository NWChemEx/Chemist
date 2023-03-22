.. Copyright 2023 NWChemEx-Project
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

###################
OrbitalSpace Design
###################

This page documents the design of the orbital space component of Chemist.

************************
What are orbital spaces?
************************

In quantum mechanics we typically form the many-particle wavefunction in terms
of one-particle wavefunctions. The one-particle wavefunctions are called
"orbitals". In quantum chemistry, we typically label the orbitals with
qualifiers to explain what they are the one-electron (on account of the Born-
Oppenheimer approximation, we usually only use orbitals for describing
electrons) wavefunction of, *e.g.*, atomic, molecular, pair-natural). An
orbital space is the vector space spanned by a particular set of
orbitals.

******************************
Why do we need orbital spaces?
******************************

Quantum chemistry equations are tensorial in nature. To actually compute
quantities we must build tensor representations. This requires choosing a
basis set. Usually each tensor mode is spanned by a particular orbital space
(although it may also be spanned by say Cartesian coordinates). The tensor
representations are then entities which live in the direct product space of
each mode's basis set. Point being, to actually build tensors we need to know
the details of the orbital spaces being used as basis sets.

****************************
Orbital Space Considerations
****************************

#. In practice, we're really designing "basis sets" (most of them just happen
   to be orbital spaces). Thus needs to support basis functions which are
   not orbitals, *e.g.*, Cartesian components.
#. Basis sets can have different types of parameters:

   - Implicit parameters (*e.g*, Cartesian basis functions)
   - Explicit parameters (*e.g.*, atomic orbitals)
   - Mix of implicit and explicit (*e.g.*, spin orbitals)
   - Base basis set and a transformation (*e.g.*, molecular orbitals)

#. Basis sets can have different properties

   - Canonical diagonalizes the Fock matrix
   - Natural orbitals diagonalize the reduced one-electron density
   - Independent orbitals imply a sparse map relationship.


********************
Orbital Space Design
********************

Figure :numref:`fig_orbital_spaces` shows the design of the orbital space
component of Chemist.

.. _fig_orbital_spaces:

.. figure:: assets/orbital_spaces.png
   :align: center

   Pieces of Chemist's orbital space component.


Example Spaces
==============

#. Molecular (spatial) orbitals: ``MOs = DerivedSpace<Tensor, AOSpace>;``
#. Atomic spin orbitals: ``SOs = ProductSpace<AOSpace, SpinSpace>;``
#. Molecular (spin) orbitals: ``DerivedSpace<Tensor, SOs>;``
#. Basis for dipole: ``ProductSpace<CartesianSpace, AOSpace, AOSpace>;``
#. Canonical molecular orbitals: ``NaturalSpace<Tensor, MOs>;``
#. Natural orbitals: ``NaturalSpace<Tensor, MOs>;``

   - Could use a strong type to distinguish from canonical MOs.
