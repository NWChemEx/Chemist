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

.. _designing_the_nucleus_component:

###############################
Designing the Nucleus Component
###############################

This section contains notes on the design of Chemist's nucleus component.

******************************
What is the Nucleus Component?
******************************

In the present context a nucleus is the core of an atom and is comprised of
neutrons and protons. The nucleus component contains the abstractions needed
to represent a single nucleus as well as a set of nuclei.

***********************************
Why do we need a Nucleus Component?
***********************************

Anecdotally most quantum chemistry packages do not bother with classes for
describing the nucleus, so why should we? There's a couple reasons. First,
particularly when it comes to manipulating the chemical system we often treat
the nuclei independent of the electrons (*i.e.* we invoke the Born-Oppenheimer
approximation). Having separate classes for the nuclei and the electrons makes
this easier as we have literal different objects we can manipulate. Second,
while the nuclei are often treated as point charges, they need not be. High
accuracy work may take into account the finite size of the nucleus and even its
quantum mechanical nature. These different descriptions
require different parameters (and therefore different classes). Our final
motivation for having a separate ``Nucleus`` class is that when it comes time
to take derivatives or define operators with respect to particles we can use
the ``Nucleus`` and ``Nuclei`` classes in the types of the derivative/operator
thus obtaining separate types for taking derivatives with respect to nuclei vs
electrons (and different  operators).

**********************
Nucleus Considerations
**********************

.. _n_basic_parameters:

basic parameters
   Nuclei are assumed to be centered on a point, have a mass, an atomic number,
   an overall charge, and an atomic symbol.

.. _n_atomic_number_v_charge:

atomic number vs. charge
   In atomic units the atomic number of a nucleus is the same as the charge.
   As such it is reasonable to not distinguish among the two; however, there
   are a couple reasons to separate them:

   - As alluded to, the two are only equal in atomic units. We intend to add
     unit support at a later time.
   - The atomic number is always an integer and is often used as the key in
     maps. While the charge is an integer in atomic units, it is still often used
     in mathematical contexts where it needs to be a floating point type. While
     integers and floating-poing values can be converted somewhat easily, having
     separate functions helps avoid compiler warnings.

.. _n_views:

Views
   Traditionally the molecular system has been stored as a structure of arrays.
   This is primarily for performance reasons. From a user-perspective an array
   of structures is easier to use. By introducing views we can have both. The
   view objects act like the values, but only alias their state.

   - We need views of ``Nucleus`` to be used as references into the ``Nuclei``
     container.
   - We need views of ``Nuclei`` to be used in containers like
     ``FragmentedNuclei`` where the elements are meant to be ``Nuclei`` objects.
   - Views also enable external libraries to wrap their data in an API
     compatible with Chemist.

Out of Scope
============

non-point charge like nuclei
   While we recognize that nuclei need not always be treated as point charges,
   given how often they are treated as point charges, we are focusing on
   point charge-like nuclei only for right now. Additional classes can be
   added to the hierarchy to handle more realistic approximations.

**************
Nucleus Design
**************

.. _fig_nucleus_component:

.. figure:: assets/nucleus.png
   :align: center

   Classes comprising the Nucleus component of Chemist.

The classes of the Nucleus component are shown in
:numref:`fig_nucleus_component`. The figure divides the classes into two
categories those which are part of the public API and those which are needed to
implement the public API. The following subsections contain more details about
the various classes.

Nucleus/NucleusView
===================

The ``Nucleus`` class is the fundamental class of the hierarchy. Consistent with
the :ref:`n_basic_parameters` consideration it will derive from ``PointCharge``.
On top of ``PointCharge``'s state we add the mass, atomic number, and atomic
symbol. The ``NucleusView`` class, consistent with :ref:`n_views` allows users
to wrap existing data in objects and have those objects be used as if they are
``Nucleus`` objects.

Nuclei/NucleiView
=================

.. _fig_nuclei_view_pimpls:

.. figure:: assets/nuclei_view_pimpls.png
   :align: center

   Class hierarchy implementing NucleiView.

Generally speaking we do not usually deal with a single nucleus, we deal with a
set of nuclei. The ``Nuclei`` object serves as a container for ``Nucleus``
objects. A separate class, versus say an STL container, is needed because
internally the ``Nuclei`` object will usually unpack the data found in the
``Nucleus`` objects it contains. This is for performance (vectorization
specifically). The ``NucleusView`` class then serves a convenient mechanism for
the ``Nuclei`` class to alias the unpacked state, while maintaining the
``Nucleus`` API.

Similar to ``NucleusView``, ``NucleiView`` is designed to facilitate using
existing data as if it were a ``Nuclei`` object. In practice, there are a lot
of different ways to bring together a set of nuclei and ``NucleiView`` will need
optimized backends for each scenario. These implementations are summarized in
:ref:`fig_nuclei_view_pimpls` and include:

- ``ContiguousNucleiView``. This is the "traditional" implementation found in
  most quantum chemistry codes. You have one array per property such that the
  :math:`i`-th element of the array is the value of that property for the
  :math:`i`-th nucleus (properties here include the accessible state of a
  ``Nucleus`` class).
- ``NucleiSubset``. Many approximations partition the system of interest into
  subsets. This backend stores a view of the nuclei from the system of interest,
  i.e., a view of the supersystem, and the members of the superset that are
  in the subset. With the superset view present, offsets for the subset members
  suffices.
- ``NucleusViewList``. Sometimes you just have a bunch of ``NucleusView``
  objects you want to treat as the elements of a ``NucleiView``. This PIMPL
  holds a vector of ``NucleusView`` objects.
- ``NucleiUnion``. A functional inverse of ``NucleiSubset``. When you have
  pieces of a system you often want to combine them. While it's always
  possible to use ``NucleusViewList`` for this purpose, it's often just easier
  to store a vector of ``NucleiView`` objects.


*******
Summary
*******

:ref:`n_basic_parameters`
   The ``Nucleus`` class contains the specified parameters.

:ref:`n_atomic_number_v_charge`
   The ``Nucleus`` object stores the atomic number as separate state. By default
   the charge is set to the atomic number (and the units are assumed to be
   atomic units).

:ref:`n_views`
  The ``Nucleus``, and ``Nuclei`` classes are paired with ``NucleusView``
  and ``NucleiView``.
