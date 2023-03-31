.. Copyright 2022 NWChemEx-Project
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

########################
Determinant Space Design
########################

.. |N| replace:: :math:`N`

Section :ref:`dd_design` explained our design for determinants, which are
basis functions for |N|-electron wavefuntions. This page describes the design
of the determinant space component of Chemist, which builds on the determinant
class hierarchy.

****************************
What is a Determinant Space?
****************************

Each determinant is a basis function for a many-electron wavefunction.
Determinant spaces are the vector spaces spanned by a particular set of
determinants.


***********************************
Why Do We Need a Determinant Space?
***********************************

The many-electron wavefunction lives in a determinant space. Like the vector
spaces in Section :ref:`vsd_design`, we are primarily interested
in the basis functions defining a particular determinant space. The various
determinant space classes

.. _ds_considerations:

********************************
Determinant Space Considerations
********************************

In designing the determinant space component of Chemist we considered the
following

.. _ds_implicit:

Implicit Excited Determinants
   Most algorithms requiring excited determinants do not require us to
   explicitly create the excited determinants. Rather, these algorithms
   make reference to a reference determinant. It is important to have
   access to this reference determinant.

.. _ds_excite_restrict:

Excitation restrictions
   For most systems, the full number of determinants which can be formed is
   a staggering amount. A number of quantum chemistry methods only consider
   determinants from a subspace of the full determinant space. These
   restrictions are usually expressed in terms of which excitations are and
   are not allowed. Examples:

   - Frozen core. No excitations involving core electrons are allowed.
   - Complete active space. Excitations are only allowed among a specified
     set of electrons and orbitals.
   - Restricted active space. Decomposes the active space into three regimes,
     each with a maximum/minimum electron occupancy

.. _ds_type_dispatch:

Type dispatch
   The determinant space classes will be used to construct property types.
   Having determinant space types for different assumptions and approximations
   allows us to ensure that module we call is aware of those assumptions and
   approximations.

**************************
Determinant Space Overview
**************************

The determinant space component is envisioned as a class hierarchy designed in
such a way that the base class represents all possible determinants which can
be formed from a particular set of orbitals, and derived classes represent
specific restrictions on which determinants are included in the space.

.. _fig_determinant_spaces:

.. figure:: assets/determinant_spaces.png
   :align: center

   Pieces of Chemist's determinant space component.

Determinant Space Class
=======================

The ``DeterminantSpace<D>`` class is the base of the hierarchy and is largely
envisioned as code factorization for the derived classes. The main state
contained in a ``DeterminantSpace<D>`` instance is the reference determinant
(which is of type ``D``). The ``DeterminantSpace<V>`` class addresses the
:ref:`ds_implicit` consideration.

.. note::

   While the idea of which orbitals are occupied is less clear cut once one
   starts going down the active space leg of the class hierarchy. Nevertheless,
   the active spaces can be thought of as excitations from some particular
   determinant. The occupations returned by the base represent this initial
   occupation.

Generalized Active Space (GAS) Space Class
==========================================

The ``GASSpace<D>`` class restricts the determinant space by defining an active
space (a specified number of electrons and a specified set of orbitals in
which those electrons may reside). Generally speaking, the active space is
partitioned into subspaces such that each subspace has a minimum and maximum
occupancy. Relative to ``DeterminantSpace<D>``, ``GASSpace<D>`` contains a list
of which orbitals are in which partitioning and the minimum/maximum number of
electrons allowed in each partition. ``GASSpace<D>`` addresses the
:ref:`ds_excite_restrict` consideration.

Active Space Class
==================

The ``ActiveSpace<D>`` is a strong type signifying that the base
``GASSpace<D>`` class has a single partitioning and the minimum/maximum number
of electrons in this space is equal to the number of active electrons. The
``ActiveSpace<D>`` class is designed primarily to address
:ref:`ds_type_dispatch`.

.. note::

   It is not uncommon to think of GAS as a simplification of CAS, which in
   turn suggests ``GASSpace<D>`` should derive from ``ActiveSpace<D>``.
   However, by defining an ``ActiveSpace<D>`` as we have (GAS with one
   partitioning, minimum/maximum occupations equal to the number of electrons)
   it should be possible to pass an ``ActiveSpace<D>`` object to a code
   expecting a ``GASSpace<D>`` object; whereas attempting the reverse (*i.e.*,
   passing a ``GASSpace<D>`` object to a code expecting an ``ActiveSpace<D>``
   object) will in general not work.


Restricted Active Space (RAS) Space Class
=========================================

.. |Nh| replace:: :math:`N_h`
.. |Ne| replace:: :math:`N_e`

The ``RASSpace<D>`` is a strong type signifying that the base ``GASSpace<D>``
has an active space with three partitions. The minimum and maximum occupancies
of each partition are controlled by two parameters: |Nh|, the maximum number of
holes allowed to be in the first partition, and |Ne|, the maximum number of
electrons allowed to be in the third partition (partitions are assumed to be
ordered by the energies of the orbitals in them). The ``RASSpace<D>`` class is
designed primarily to address :ref:`ds_type_dispatch`.

Frozen Core Class
=================

The ``FrozenCore<B>`` class represents that the occupied orbitals are
partitioned into two regimes, a regime from which no excitations are allowed
(*i.e.*, the core) an a regime from which excitations are allowed. The class is
a mix-in that is templated on the determinant space it derives from. For
example ``FrozenCore<ClosedShell<V>>`` denotes the determinant space possible
given a set of restricted orbitals, but not allowing core excitations.

Determinant Spaces Examples
===========================

So far all of the determinant spaces we have listed have been class templates.
In practice, we expect that users will somewhat rarely deal with the templates
and will instead usually deal with explicit specializations. Some examples are
given below (``T`` is a tensor type):


*************************
Determinant Space Summary
*************************

To summarize how our current design addresses the concerns raised in
:ref:`ds_considerations`:

:ref:`ds_implicit`
   The ``DeterminantSpace<D>`` class only holds the reference determinant.
   Derived classes represent the allowed excited determinants by storing
   parameters reflecting which excitations are allowed (*e.g.*, only doubles,
   or only within an active space) or via strong types.

:ref:`ds_excite_restrict`
   ``FrozenCore<B>`` was designed to signal that core excitations are not
    allowed. More fine-grained restrictions fall to the ``GASSpace<V>`` (and
    its subclasses).

:ref:`ds_type_dispatch`
   The hierarchy contains a number of strong types which can be used to
   distinguish between assumptions used to construct the spaces.
