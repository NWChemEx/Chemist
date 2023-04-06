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

###############
Molecule Design
###############

This section contains notes on the design of Chemist's molecule component.

*******************
What is a Molecule?
*******************

In chemistry molecules are sets of atoms which are covalently bonded. In
computational chemistry, particularly quantum chemistry, the covalently-bonded
requirement is usually dropped, *i.e.*, a molecule is simply a set of atoms.
Our present discussion assumes the computational chemistry definition.

**************************************
Why do we need a Molecule (component)?
**************************************

Computational chemistry aims to predict chemical phenomena. Part of the input
to a computational chemistry package will always be some sort of chemical
system. The molecule is used to represent the atomic part of that chemical
system. Thus we need a molecule component so that users can specify the atoms
in their system.

.. _molecule_considerations:

***********************
Molecule Considerations
***********************

The concept of a molecule class, is widely ingrained in most, if not all,
object-oriented computational chemistry packages. As such, many developers
have an implicit idea of what the molecule should do. In this section we
explicitly list the considerations which went into our molecule component.

.. _md_data_structure:

Data structure.
   Chemist is designed so that classes are primarily data structures.
   Manipulations of data structures are primarily done via functions (modules
   particularly).

.. _md_nuclei_reuse:

Nuclei reuse.
   Typical quantum chemistry uses see the nuclei being treated as point
   charges. Ideally the infrastructure underlying the nuclei reuses the
   infrastructure for point charges.

.. _md_quantum_electrons:

Quantum electrons.
   Typical quantum chemistry use treats electrons as quantum mechanical
   particles. The main repercussion of this is that while the number of
   electrons of an isolated atom is well-defined, when atoms come together
   it becomes impossible to unambiguously assign electrons to atoms, and
   we must talk about "the molecule's electrons" not "an atom's electrons".
   Sub-considerations:

   - Charge and spin properties of ``Molecule``
   - ``Molecule`` really shouldn't be a set of atoms, since that would preserve
     the electron to atom assignments. Should instead contain a set of nuclei.

.. _md_performance:

Performance.
   Thinking of molecules as a set of atoms suggests that an array-of-structures
   is a natural representation. However, it is also widely known that because
   of modern hardware's reliance on vectorization, a structure-of-arrays
   will perform better.

Not In Scope
============

Given the prevalence of the molecule class in other software packages, we have
also explicitly listed other design points which were considered, but we
ultimately decided to be out of scope for the molecule component.

Hierarchical structure.
   There are a number of electronic structure techniques (*e.g.*, many-body
   expansion, symmetry-adapted perturbation theory, or basis-set superposition
   error) which require one to decompose the system of interest into subsystems.
   While valid use cases for the an electronic structure package, we have opted
   to treat these cases at a higher level. Reasoning:

   - Any hierarchical approach will necessarily need a molecular representation
     of each subsystem.
   - We require a non-hierarchical structure which can be used by methods
     which will assume a non-hierarchical input, *i.e.*, we want a class that
     guarantees that the input should be treated in a non-hierarchical manner.

Basis set.
   Most electronic structure packages consider the atomic basis set to be an
   input to the package. For methods relying on atom-centered basis functions,
   the conversion from atomic basis set to molecular basis set is thus
   intrinsically linked to the molecule by virtue of the fact that the basis
   functions reside at the same point in space as the nuclei. We have opted to
   represent the atomic and molecular basis sets with separate classes, and not
   have them be part of the molecule component. Reasoning:

   - In practice the molecule is used to construct the molecular basis set.
     Once the molecular basis set is created, the vast majority of
     infrastructure needs the basis set, not the molecule (the molecule is
     minimally still needed for Hamiltonian terms involving the nuclei). Thus
     separating them helps separate concerns.
   - Atom-centered basis sets are not the only basis sets in common usage. In
     particular plane-waves are commonly used. Tying atom-centered basis sets
     to the molecule would create an awkward situation where plane-wave based
     codes would have to ignore/error if the user sets that state.

Connectivity.
   It is not uncommon to want some notion of which atoms are bonded to each
   other. That said, there are a number of ways to actually determine such
   connectivity, suggesting it would be better to create modules for each
   possible algorithm for mapping a molecule to a connectivity table. Given the
   somewhat non-trivial nature of establishing connectivity, requiring the user
   to provide a connectivity table is heavy-handed. Similarly, tying the
   molecule to a particular algorithm is unlikely to satisfy all users; thus,
   we have opted to represent the connectivity of a molecule with an entirely
   different object (the ``ConnectivityTable``).

Topology.
   Here we define the topology of a molecule as the distances, angles, and
   torsion angles between and among the atoms in the molecule. Following from
   arguments nearly identical to those presented for the connectivity, we
   have opted to store the topology in a different class as well (the
   ``Topology`` class).`

***************************
Overview of Molecule Design
***************************

.. _fig_molecule_design:

.. figure:: assets/molecule.rst
   :align: center

   Major pieces of the molecule component of Chemist.

Subject to the considerations raised in :ref:`molecule_considerations`, the
overall design of the molecule component is shown in
:numref:`fig_molecule_design`. The component is comprised of two related
class hierarchies. The point hierarchy describes the individual descretized
units of the system, whereas the point-set hierarchy represents the overall
set of descretized units.

Point Hierarchy
===============

In addressing the :ref:`md_nuclei_reuse` consideration, we have opted to design
the ``Atom`` class as a derived class in a hierarchy. This allows each of the
base classes to be reused for other concepts. The design of the ``Atom`` and
its base classes are summarized in the following subsections.

Point Class
-----------

In scientific simulations we often have a number of point-like objects (point
multipole moments, point masses, grid points, etc.). While we rarely only
care about the location of these points, *i.e.*, each point also usually has
additional state, these point-like objects do all share the common property of
being located somewhere in Cartesian space. The ``Point`` class has been
introduced to represent such objects.

The ``Point`` class primarily serves as:

- code-factorization for state common to the various point-like objects,
- a means of writing generic algorithms which only require knowledge of an
  object's coordinates

Point Charge Class
------------------

When we associate a charge with a point in space we get a point charge, and the
``PointCharge`` class is designed to represent such a charge. Relative to the
``Point`` class, the ``PointCharge`` class adds the charge of the point charge.

The ``PointCharge`` class serves as:

- code-factorization for state common to point-charge-like objects including
  both point-charges, and more relevantly, nuclei,
- a means of writing generic algorithms that work with any point charge, be it
  nuclei or actual point charges.

Nucleus Class
-------------

Quantum chemistry overwhelming views nuclei as massive point-charges. The
``Nucleus`` class adds mass and atomic number to the ``PointCharge`` base
class. In practice, given consideration :ref:`md_quantum_electrons`, we
expect ``Nucleus`` objects to be the descretization of the ``Molecule`` class,
and thus much of the state of ``Molecule`` will be tied up in ``Nucleus``
objects. There are a couple points to note about the design:

- Despite inheriting charge from the base class, and despite the fact that (in
  atomic units) the charge is usually the same as the atomic number, we have
  also added the atomic number. At the very least separating the two provides
  different methods for retrieving the value as a ``double`` vs.
  ``std::size_t``. Another potential reason was alluded to by the parenthetical,
  namely the equality between atomic number and charge is only true in atomic
  units. If we add unit literals (as planned), the charge depends on the unit,
  but the atomic number does not.
- We opted to have ``Nucleus`` add BOTH mass and atomic number to avoid
  multiple inheritance. Arguably, inheriting from ``PointCharge`` and
  a ``PointMass`` class would have better symmetry in the sense that each
  class would add only one property; however, since in atomic units the
  electron has a mass of 1, the only point masses we routinely need
  the mass of are nuclei. Hence we could not justify the extra class at
  this time.

Atom Class
----------

In practice, when people build molecules they do so by specifying atoms. In
electronic structure theory, an atom is a set of electrons and a nucleus. We
introduce the ``Atom`` class to represent isolated atoms. We primarily see
the ``Atom`` class serving as:

- a means of building ``Molecule`` objects by ``push_back``-ing ``Atom``
  objects, and
- a potential building block of a "classical" molecule object by, for example,
  setting the number of electrons off of a charge partitioning scheme such as
  Mulliken or Lowdin charges.

Of some note in our design, ``Atom`` contains a ``Nucleus`` and does not
inherit from it. This is because atoms are in general not just nuclei (the
notable exceptions being :math:`H^+` and  :math:`He^{2+}`), but are nuclei AND
electrons.

Point Set Hierarchy
===================

The point set hierarchy mirrors the point hierarchy, except that each class
contains one or more of its namesake descretized units. As explained in more
detail below, the introduction of new classes (as opposed to simply just using
``std::vector<Point>``, for example) is in anticipation of the
:ref:`md_performance` consideration. More specifically, by having actual
classes we are able to define an API, which is decoupled from the
implementation, whereas if we declare ``Molecule`` as a typedef of
``std::vector<Atom>``

Point Set Class
---------------

When one has more than one ``Point`` object, we say they have a set of points.
The ``PointSet`` class is introduced to represent a container of ``Point``
objects. In particular ``PointSet`` is envisioned as:

- providing an array-of-structures API, potentially by returning ``PointView``
  objects (objects which act like ``Point`` instances, but do not own their
  state),
- having a structure-of-arrays implementation (to facilitate vectorization)
- be an ordered set (points should appear at most once and be index-able)

Charges Class
-------------

The ``Charges`` class is simply the generalization of the ``PointSet`` class.
The main design points:

- charges stored contiguously,
- could introduce ``PointChargeView`` for an array-of-structures API
- presumably adds a method for computing the total charge of the point charges.

Nuclei Class
------------

When our container contains ``Nucleus`` objects, the container is now a
``Nuclei`` object. Notably:

- deriving form ``Charges`` satisfies :ref:`md_nuclei_reuse`,
- mass and atomic numbers can be collected into contiguous arrays,
- ``NucleusView`` for maintaining an array-of-structures API.

Molecule Class
--------------

The namesake of the molecule component, the ``Molecule`` class is the
culmination of the design so far. Notably the ``Molecule`` class:

- acts like an array of ``Nuclei`` objects consistent
  with :ref:`md_quantum_electrons`,
- stores the electronic charge of the system (the net number of
  electrons), which is different than the total charge of the nuclei,
- stores the multiplicity of the electrons
- access to the nuclei as a ``Nuclei`` object addresses :ref:`md_nuclei_reuse`

***********************
Molecule Design Summary
***********************

To summarize how Chemist's molecule component addresses the considerations
raised in :ref:`molecule_considerations`:

:ref:`md_data_structure`
   This consideration factored more into deciding what was and was not part of
   the molecule component. In particular, the desire for the classes to be
   data structures precluded inclusion of complicated methods, such as those
   for determining topology or connectivity.

:ref:`md_nuclei_reuse`
   The fact that ``Nucleus`` and ``Nuclei`` respectively derive from
   ``PointCharge`` and ``Charges`` class, means the classes can be used
   seamlessly wherever ``PointCharge`` and ``Charges`` are used.

:ref:`md_quantum_electrons`
   The ``Molecule`` class contains a set of ``Nucleus`` objects, not a set of
   ``Atoms``. This reflects the fact that once we create the ``Molecule`` we
   can no longer, unambiguously, assign electrons to centers. The (quantum)
   electrons in the ``Molecule`` class are stored implicitly via charge
   and multiplicity.

:ref:`md_performance`
   By opting for full-fledged classes for the sets in the component, we are
   able to separate the API of the class from its implementation, which would
   not be as easy if we had opted to represent a molecule as a
   ``std::vector<Atom>`` for example.
