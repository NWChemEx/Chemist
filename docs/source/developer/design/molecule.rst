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

***********************
Molecule Considerations
***********************

The concept of a molecule class, is widely ingrained in most, if not all,
object-oriented computational chemistry packages. As such, many developers
have an implicit idea of what the molecule should do. In this section we
explicitly list the considerations which went into our molecule component.

Data structure.
   Chemist is designed so that classes are primarily data structures.
   Manipulations of data structures are primarily done via functions (modules
   particularly).

Nuclei Reuse
   Typical quantum chemistry uses see the nuclei being treated as point
   charges. Ideally the infrastructure underlying the nuclei reuses the
   infrastructure for point charges.

Quantum Electrons
   Typical quantum chemistry use treats electrons as quantum mechanical
   particles. The main repercussion of this is that while the number of
   electrons of an isolated atom is well-defined, when atoms come together
   it becomes impossible to unambiguously assign electrons to atoms, and
   we must talk about "the molecule's electrons" not "an atom's electrons".
   Sub-considerations:

   - Charge and spin properties of ``Molecule``
   - ``Molecule`` really shouldn't be a set of atoms, since that would preserve
     the electron to atom assignments. Should instead contain a set of nuclei.

Not In Scope
============

Given the prevelance of the molecule class in other components, we have also
explicitly listed other design points for the molecule component which were
considered, but ultimately decided to be out of scope.


***************************
Overview of Molecule Design
***************************
