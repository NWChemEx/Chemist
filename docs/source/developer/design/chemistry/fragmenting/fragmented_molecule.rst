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

.. _designing_fragmented_molecule_class:

######################################
Designing the FragmentedMolecule Class
######################################

.. |n| replace:: :math:`n`

``Fragmented<Molecule>`` tracks how the ``Molecule`` piece of the 
``ChemicalSystem`` is decomposed. This page discusses the design of that class.

******************************
What is a fragmented molecule?
******************************

In computational chemistry parlance, a molecule is the set of atoms we are
running a computation on. Sometimes we need to describe a "piece of a 
molecule." These pieces are fragments, and the set of these fragments is the
fragmented molecule.

************************************
Why do we need fragmented molecules?
************************************

In computational chemistry there are a number of methods which require breaking
the molecule up into two or more pieces. While at some point many of these
methods will rely on the fact that the fragments are themselves perfectly good
molecules, most times the algorithm will also need to use the fact that the
fragments are pieces of a larger system. In turn, we need a way to represent
the fragments, while not forgetting the molecule the fragments come from.

***************************************
FragmentedMolecule Class Considerations
***************************************

.. _fm_molecule:

Molecule compatibility.
   Chemist defines the ``Molecule`` class. The ``Fragmented<Molecule>`` class
   should behave like a container of ``Molecule`` objects. The 
   ``FragmentedPIMPL<Molecule>`` class is responsible for implementing this.

.. _fm_charge_mult:

Charge and multiplicity.
   In fragmenting the molecule, the charge/multiplicity must also be fragmented.
   More specifically we must assign a charge/multiplicity to each fragment.

*************************
FragmentedMolecule Design
*************************

TODO: update.