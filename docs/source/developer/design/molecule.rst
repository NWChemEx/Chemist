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
base classes to be reused for other concepts.

Point Class
-----------

In scientific simulations we often have a number of point-like objects (point
multipole moments, point masses, grid points, etc.). While we rarely only
care about the location of these points, *i.e.*, each point also usually has
additional state, these point-like objects do all share the common property of
being located somewhere in Cartesian space. The ``Point`` class is 
code-factorization for the various point-like objects and contains the
infrastructure needed for managing the Cartesian coordinates of the point.


***********************
Molecule Design Summary
***********************

:ref:`md_data_structure`

:ref:`md_nuclei_reuse`

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