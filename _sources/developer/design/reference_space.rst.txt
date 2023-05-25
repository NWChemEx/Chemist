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

.. _rsd_design:

######################
Reference Space Design
######################

This page describes the design of the reference space component of Chemist.

**************************
What is a Reference Space?
**************************

.. |N| replace:: :math:`N`
.. |i| replace:: :math:`i`
.. |j| replace:: :math:`j`
.. |M| replace:: :math:`\mathbf{M}`
.. |Mij| replace:: :math:`M_{ij}`

Many-electron wavefunctions are expressed in terms of many-electron basis
functions. We define a reference space as the space of many-electron basis
functions which can be used to form a wavefunction. Arguably the most important
many-electron basis functions are written as determinants.

Our present interest in determinants is purely from a quantum chemistry
perspective, not the general mathematical angle. In the context of
quantum chemistry determinants are basis functions for |N|-electron
wavefunctions. The name determinant comes from the fact that if you
create an |N| by |N| matrix, |M|, such that element |Mij| is the |i|-th
electron occupying the |j|-th orbital, then the determinant of |M| is a
properly antisymmetrized |N|-electron basis function.

.. note::
   One obtains the same result by instead defining |Mij| as
   the |j|-th electron in the |i|-th orbital, since
   :math:`det\left(\mathbf{M}\right)=det\left(\mathbf{M}^T\right)`.

*********************************
Why Do We Need a Reference Space?
*********************************

Vector spaces describe basis sets for one-electron wavefunctions, reference
spaces generalize these concepts to many-electron wavefunctios. In particular,
the reference space component provides us a representation of the space our
wavefunction will live in, but more notably the reference space
will provide us the basis vectors we will build our wavefunction from.

.. _rsd_considerations:

******************************
Reference Space Considerations
******************************

.. _rsd_ov:

Occupied/Virtual
   As mentioned in the previous section the determinant class is primarily
   envisioned as a means of storing how we partitioned the orbitals into
   occupied and virtual orbitals. In practice, most methods implicitly create
   all determinants they will need by referring to the original partitioning.
   Hence we are primarily interested in using the determinant component to
   track the reference determinant.

.. _rsd_occ_restrict:

Occupation Restriction
   Knowing whether each orbital is restricted to doubly occupied, or if there
   are some singly occupied orbitals, can lead to algorithm simplifications.
   The class hierarchy should make it possible to glean this information from
   the type alone.

.. _rsd_csf:

Configuration state functions (CSFs)
   In general a single determinant is not capable of satisfying the spin
   symmetries of an arbitrary system (the notable exception being high-spin
   systems). CSFs are eigenfunctions of the spin-projection operator.


.. _rsd_product:

Product spaces.
   There are methods (*e.g.*, symmetry-adapted perturbation theory) which use
   products of determinants as basis functions for the many-electron
   wavefunctions. The catch is that the resulting basis functions are only
   antisymmetric within each determinant, not overall (which is what would
   happen if one started with tensor product of the orbital spaces and then
   took the determinant).

*************************************
Overview of Reference Space Component
*************************************

.. _fig_ref_space:

.. figure:: assets/reference_space.png
   :align: center

   The fundamental classes comprising Chemist's reference space component.

:numref:`fig_ref_space` shows the major pieces of Chemist's reference space
component. Compared to :ref:`vsd_design`, the reference space component is
relatively simple; a fact that stems from realizing that most of the variation
in modern electronic structure methods comes from how to choose the orbitals,
or how to approximate the many-electron wavefunction. In other words, there's
comparatively few ways to create |N|-electron basis functions from orbitals.

Reference Space Class
=====================

The base class of the hierarchy is the ``ReferenceSpace<V>`` class
(here ``V`` is the type of a vector space representing the orbital space).
To address the :ref:`rsd_ov` consideration, this class stores a reference to
the (full) orbital space the basis functions can be created from (*i.e.*, it
is not just the occupied orbitals) and the orbital occupations.
Admittedly, in many cases, orbital occupations are somewhat trivial to
store implicitly and developers of the ``ReferenceSpace<V>`` class may opt for
multiple backends depending on whether or not the occupations need to be
explicitly stored (usually only necessary for excited states or fractional
occupations).

Product Space Class
===================

Consideration :ref:`rsd_product` leads to needing a class for reference spaces
that are formed from products of other reference spaces. We introduce the
``ProductSpace<R...>`` class for this purpose. It is assumed that
``ProductSpace<R...>`` will only be used when there are at most a few parameters
in the parameter pack. If one needs a ``ProductSpace<R..>`` of a large number
of spaces (or an indeterminate number), additional classes should be
added.


CSF Class
=========

At this stage the ``CSF<V>`` class is not very fleshed out. We simply note that
CSFs can be used as alternatives to determinants in many theories. The advantage
of CSFs is that they are eigenfunction of the spin-projection operator
regardless of whether or not we have a high-spin wavefunction. The disadvantage
is they are significantly more complicated than determinants. The addition of
``CSF<V>`` directly addresses the :ref:`rsd_csf` consideration.

Determinant Class
=================

The ``Determinant<V>`` class is a strong type guaranteeing that the basis
functions in the reference space are single determinants.

High Spin Class
===============

A number of electronic structure methods assume that the determinant is in a
high-spin state (all unpaired electrons have the same spin). While we could
carry around the spins of each electron, and determine on-the-fly if we have
a high-spin state, we instead have created a strong type ``HighSpin<V>`` to
denote when a determinant is in a high-spin state. This is justified by the
fact that the user makes this decision (implicitly) based on what method they
select. Combined with the ``ClosedShell<V>`` class introduced below, the
``HighSpin<V>`` class addresses the :ref:`rsd_occ_restrict` consideration.

Closed Shell Class
==================

A special (trivial) case of a high-spin determinant is one with no unpaired
electrons. Knowing that there are no unpaired electrons again leads to
simplifications in the resulting theories, which is why we opt to denote this
condition by the ``ClosedShell<V>`` strong type. Together with the
``HighSpin<V>`` class, ``ClosedShell<V>`` addresses the :ref:`rsd_occ_restrict`
consideration.

Example Reference Spaces
========================

The remainder of :numref:`fig_ref_space` shows some template specializations
and strong types for those specializations. We have opted for strong types,
rather than typedefs, primarily to minimize the use of templates in user-facing
classes (which also has the advantage of cleaner compiler errors). Briefly,
the strong types we introduce are:

- ``UDeterminant<T>``. Class denoting that determinant is expressed in terms
  of spin orbitals, occupied in a high-spin state. These are the determinants
  considered in methods typically labeled as "unrestricted".
- ``RODeterminant<T>``. Same as ``UDeterminant<T>``, except that the orbitals
  are spatial orbitals (not spin). These are the determinants considered in
  methods typically labeled as "restricted open-shell".
- ``RDeterminant<T>``. Same as ``RODeterminant<T>``, except that we additionally
  know that there are no singly-occupied orbitals. These are the determinants
  considered in methods typically labeled as "restricted".
- ``SAPT2BSpace<T>``. Denotes the reference space is the tensor product of two
   (closed-shell) monomer wavefunctions.

Also denoted in :numref:`fig_ref_space` is that ``RDeterminant<T>`` should be
implicitly convertible to an object of type ``RODeterminant<T>``, which itself
should be implicitly convertible to an object of type ``UDeterminant<T>``.
The decision to use implicit conversions is to avoid multiple inheritance, but
still allow physically meaningful conversions (a ``RDeterminant<T>`` is just
a special case of an ``RODeterminant<T>``, which is just a special case of a
``UDeterminant<T>``).

******************************
Reference Space Design Summary
******************************

The design of the determinant component satisfies the considerations raised in
section :ref:`rsd_considerations` by:

:ref:`rsd_ov`
   The ``Determinant<V>`` class stores a reference to the full orbital space
   and the occupation vector.

:ref:`rsd_occ_restrict`
   The strong types ``HighSpin<V>`` and ``ClosedShell<V>`` add occupation
   restriction information to the type, facilitating type dispatching based
   on how the orbitals are occupied.

:ref:`rsd_csf`
   The ``CSF<V>`` class was added to the hierarchy to ensure we have a means
   of representing many-electron spaces which use CSFs as basis functions.

:ref:`rsd_product`
   The ``ProductSpace`` class allows us to form reference spaces which are
   products of other reference spaces.
