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
Vector Space Design
###################

This page documents the design of the vector space component of Chemist. In
practice this component really focuses in on the basis set defining the vector
space; however, in quantum chemistry the term "basis set" is usually associated
specifically with quantities like `6-31G*` and we wish to avoid confusion.

************************
What are vector spaces?
************************

One can find mathematical descriptions of a vector space
`here <https://en.wikipedia.org/wiki/Vector_space>`__. For our purposes, what
we really care about is that in order to practically represent vectors in a
given vector space one needs to define a basis set.

*****************************
Why do we need vector spaces?
*****************************

Quantum chemistry equations are tensorial in nature. To actually compute
quantities we need tensor representations (*i.e.*, arrays of numbers). Forming
a tensor representation requires us to pick a basis set (in the general sense,
*i.e.*, we are not limiting ourselves to things like `6-31G*`). For a given
vector space there are usually an infinite number of basis sets, and
unfortunately each (in general) results in a different tensor representation.
The point being, in order to programatically express which basis set we are
using, we need a class (or series of classes) to convey this information.


.. _vsd_considerations:

***************************
Vector Space Considerations
***************************

The following lists the considerations which went into designing the vector
space component of Chemist:

.. _vsd_basis_set:

Basis Set
   In practice, we're really designing basis sets (all of which can be used
   to define vector spaces, but that's not really what we care about). The
   vector space component needs to support arbitrary basis functions, not just
   basis functions which are orbitals. Notable examples:

   - Cartesian axes
   - Spin

.. _vsd_parameters:

Parameters
   Basis sets have different basis functions, we need a means of telling the
   basis functions a part. Typically this is done by referring to the
   parameter values for a particular basis function. Parameters come in a
   variety of types:

   - Implicit. Usually defined by conventional labels, *e.g*, the "x"-axis of a
     Cartesian basis set, or the :math:`\alpha` spin function.
   - Explicit. Many basis functions are defined by explicit numeric parameters,
     *e.g.*, primitive Gaussian functions, or plane waves.
   - Mix of implicit and explicit. These are basis functions which are defined
     in terms of both implicit and explicit parameters, *e.g.*, spin orbitals,
     or the basis set for dipole moment integrals.
   - Transformation from another basis set, *e.g.*,
     contracted Gaussian type orbitals, or molecular orbitals.

.. _vsd_properties:

Properties
   Basis sets are usually not chosen at random. Rather, one typically chooses a
   basis set which satisfies a particular property. Notable examples
   include:

   - Canonical molecular orbitals, which diagonalize the Fock matrix.
   - Natural molecular orbitals, which  diagonalize the one-electron density
     matrix.
   - Independent/dependent orbitals which have a sparse map relationship.

.. _vsd_type_dispatch:

Type Dispatch
   Programatically the vector spaces will be used to construct property types.
   The types for the vector space should aim for generality, while still
   containing sufficient information to ensure that a property type can only
   be called with vector spaces possessing the correct properties. Examples:

   - If an algorithm assumes that the provided orbitals are canonical
     molecular orbitals, we in general do not want to call such an algorithm
     with localized orbitals.

********************
Vector Space Design
********************

The design of the vector space component is based primarily on the
considerations listed in section :ref:`vsd_considerations`.

.. _fig_orbital_spaces:

.. figure:: assets/orbital_spaces.png
   :align: center

   Pieces of Chemist's vector space component. See text for more details and
   note that most web browsers allow you to view the image in a new tab, which
   allows you to zoom in for better viewing.

:numref:`fig_orbital_spaces` shows the design of the orbital space
component of Chemist. The figure admittedly contains a large number of nodes;
however, most of these nodes are either partial specializations of a class
template (in which case they show up in the graph simply to indicate which
specialization a class derives from) or strong types (a class whose state is
simply its type). The complexity of the class graph is necessary if we want to
satisfy consideration :ref:`vsd_type_dispatch`.


Base Class
==========

At the base of the class hierarchy is the ``BaseSpace`` class. This class is
primarily meant to provide code factorization for the derived classes. It is
also useful in a few places where we try to write very generic algorithms
(*e.g.*, for transforming tensors).

Cartesian Space
===============

.. |N| replace:: :math:`N`

The ``CartesianSpace`` class is meant to represent |N|-dimensional
Cartesian space, *i.e.*, :math:`\mathbb{R}^N`. In practice, this class's only
state is the value of |N|. The need for ``CartesianSpace`` stems from
Consideration :ref:`vsd_basis_set`.

Spin Space
==========

Conceptually similar to ``CartesianSpace``, the ``SpinSpace`` class represents
the space spanned by a series of spin functions. In the most common scenario,
an instance of ``SpinSpace`` represents the spins of an electron and thus
contains two functions. State of the ``SpinSpace`` class is the total spin,
which would be 1/2 for an electron. The need for this class also stems from
consideration :ref:`vsd_basis_set`.

Atomic Orbitals (AOs)
=====================

The ``AOSpace`` represents the space spanned by the atomic orbitals which can
be generated from the molecular basis set (here basis set is used in the
computational chemistry sense). The ``AOSpace`` is a very important space for
most calculations because it represents a sort of "most fundamental space"
from which all other orbital spaces are usually derived. The need for the
``AOSpace`` class primarily stems from consideration :ref:`vsd_parameters`.

It is worth noting that in practice what we call AOs are often transformed,
contracted Gaussian type orbitals. In theory we could better consolidate the
``AOBasisSet`` class (which describes the parameters associated with those
orbitals) with the various vector space classes by defining:

- ``PrimitiveGaussianSpace`` holds the centers, angular momenta, and exponents
  for each primitive (Cartesian) Gaussian function.
- ``CGTOSpace=TransformedSpace<T, PrimitiveGaussianSpace>`` stores the
  contraction coefficients for going from primitive Gaussians to contracted
  (Cartesian) Gaussian type orbitals.
- ``SphericalGTOSpace = TransformedSpace<T, CGTOSpace>`` holds the spherical
  transform for going from contracted (Cartesian) Gaussian type orbitals to
  spherical Gaussian type orbitals.

That said, most electronic structure packages have a class/concept similar to
our ``AOBasisSet`` class and thus inter-package interoperability is facilitated
by having an ``AOBasisSet`` class. Nonetheless, there are still use cases which
could benefit from deviating from the standard ``AOBasisSet`` concepts, *e.g.*,
relativistic calculations which typically un-contract the contracted Gaussians.
It may be worth revisiting the design of the ``AOBasisSet`` class if one is
interested in use cases which use more fundamental orbitals than the ones
represented by the ``AOSpace``.

Derived Space
=============

Many of the spaces we are interested in our defined as linear transformations
of one another. The ``DerivedSpace<T,R>`` class represents a space obtained by
transforming a space of type ``R`` (``R`` is typically ``AOSpace``) by a
transformation of type ``T`` (``T`` is a tensor-like type). Usually the
transformation is a rotation (meaning it preserves lengths and angles of the
space being transformed), but we don't strictly enforce this (one could). In
practice, the ``DerivedSpace<T, R>`` class primarily serves as code
factorization for the variety of spaces which are defined as linear
transformations; by introducing ``DerivedSpace<T, R>``, many of those spaces
are simply strong types.

Molecular Orbitals (MOs)
========================

The partial specialization of ``DerivedSpace<T, R>`` for which ``R`` is an
``AOSpace`` is so common we assign a strong type to it. That resulting class
is ``MOSpace<T>``. There are a number of algorithms which only care about
whether or not a particular vector space is an ``AOSpace``, or a transformation
of the ``AOSpace`` (*e.g.*, the algorithm for transforming a tensor from the
AO basis set). These algorithms do not care if the transformation is to
canonical MOs, to localized MOs, or to something else.

Localized MOs
=============

``LocalizedMOs<T>`` is a strong type used to denote that the MOs have been
spatially localized according to some metric.

Product Space
=============

There are several important vector spaces which are obtained by taking tensor
products of other spaces. The ``ProductSpace<R...>`` class represents a
space resulting from the product of the spaces ``R...`` (assumed to be two or
more other spaces). Like ``DerivedSpace<T, R>``, ``ProductSpace<R...>``` is
introduced as a means of code factorization so that the derived classes
become strong types.

Spinor Space
============

.. |alpha| replace:: :math:`\alpha`
.. |beta| replace:: :math:`\beta`
.. |2N| replace:: :math:`2N`

The ``SpinorSpace<T>`` class is a strong type of a derived space whose
reference state is the space of atomic spin orbitals (product of an AO and a
spin function). For |N| AOs this means we have |2N| atomic spin orbitals,
which get transformed into |2N| new orbitals. In turn we have a |2N| by |2N|
transformation matrix.

Spin Orbitals (SOs)
===================

Conceptually similar to the ``SpinorSpace<T>``, the ``SOSpace<T>`` differs in
that it excepts the resulting orbital transformation to be restricted so that
the |alpha| AOs are only mixed with other |alpha| AOs and the |beta| AOs are
only mixed with other |beta| AOs`. In turn, each of the resulting |2N| spin
orbitals are defined in terms of |N| coefficients and our transformation is
|N| by |2N|, not |2N| by |2N| like in the spinor case.

Natural Space
=============

There are a number of orbital spaces where the transformation is defined by
requiring the resulting orbitals to diagonalize a particular tensor. In these
cases the resulting orbitals are often said to be the "natural" basis set on
account of the fact that they simplify equations involving the diagonalized
quantity. Compared to a ``DerivedSpace<T, R>`` object, the additional state
contained in a ``NaturalSpace<T, B>`` is the diagonalized tensor (which is
assumed to be stored in a tensor of type ``T``). Rather than deriving directly
from ``DerivedSpace<T,R>`` we allow the user to specify the base class to
derive from (this allows one to derive a natural space from an ``MOSpace<T>``
or an ``SOSpace<T>``, for example). The need for this space stems from
:ref:`vsd_properties`.


Canonical Molecular Orbitals (CMOs)
===================================

In conventional electronic structure theory, the CMOs, *i.e.*,  the orbitals
which diagonalize the Fock matrix, are one of the most important natural
spaces on account of the fact that they simplify the derivation of
correlated methods (although they notably do not simplify the computational
cost of such methods). The ``CMOSpace<T>`` class is a strong type to denote
that the additional tensor contained in the base
``NaturalSpace<T, MOSpace<T>>`` contains the orbital eigenvalues and that the
orbitals diagonalize the Fock matrix.

Natural Orbitals
================

Natural orbitals diagonalize the one-electron density matrix and the
``NOSpace<T>`` class was designed to represent that the tensor contained in
the base ``NaturalSpace<T, MOSpace<T>>`` class contains the orbital occupation
values.

Independent Spaces
==================

In theories exploiting (usually spatial) sparsity, members of one basis
set usually only have non-zero tensor elements with some of the members of
another basis set. This defines a "sparse map" where each member of the first
basis set is associated with a "domain" of the second set. The first set
is termed the "independent space" and the second basis set is termed the
"dependent space". The ``IndependentSpace<B>`` class serves as
code-factorization for implementing vector spaces for which we have sparse
maps. Relative to the base class ``B`` it inherits from,
``IndependentSpace<B>`` includes a sparse map.

********************
Vector Space Summary
********************

Over the years, electronic structure theorists have used a lot of basis sets.
Each basis set has different properties, which in turn can influence the
approximations and assumptions algorithms consuming the basis sets can make.
The vector space component was introduced to represent those basis set in the
simplest manner possible. The vector space component was designed adhering to
the considerations listed in :ref:`vsd_considerations`. The responsiveness of
our design to those considerations is summarized below.

:ref:`vsd_basis_set`
   The design includes a number of non-orbital vector spaces and the
   ``BaseSpace`` class does not assume that classes which derives from it
   actually contain orbitals.

:ref:`vsd_parameters`
   The class hierarchy derives a new class anytime the definition of the
   basis set depends on a new set of parameters.

:ref:`vsd_properties`
   Classes representing generic (and specific) basis set properties are found
   throughout the hierarchy. Functions are encouraged to use the lowest level
   (*i.e.*, closest to ``BaseSpace``) necessary in order to implement their
   algorithm.

:ref:`vsd_type_dispatch`
   Ultimately similar to :ref:`vsd_properties`, by establishing a series of
   strong types throughout the hierarchy it is possible to distinguish between,
   say canonical molecular orbitals and natural orbitals by type alone. In
   turn, if a module, for example, wants to assume it has been provided CMOs,
   it can do so by relying on C++'s type system.

*********************
Future Considerations
*********************

- The ``IndependentSpace<B>`` class is envisioned as being used by a whole
  host of additional spaces. The design should be fleshed out to accommodate
  those spaces when the time comes.
