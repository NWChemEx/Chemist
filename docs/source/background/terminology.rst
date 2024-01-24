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

#############################
Terminology and Abbreviations
#############################

.. glossary::

   atomic basis set
      An atomic (electronic) basis set describes the electrons on an isolated
      atom. Strictly speaking labels like "6-31G*" and "aug-cc-pVDZ" refer to
      atomic basis sets. Use of labels like "6-31G*" to label molecular basis
      sets is justifiable because there is often a straightforward mapping from
      the atomic basis set to the molecular basis set (see
      :term:`molecular basis set` for more information).

   chemical system
      We use the term "chemical system" to refer to chemistry objects being
      simulated. A chemical system includes the set of atoms and the fields
      the atoms are embedded in. It's the model of the experiment.

   fragment
      A distinction applied to a set of nuclei, a molecule, or a
      :term:`chemical system` which establishes that the set of nuclei,
      molecule, or chemical system is a subset of another set of nuclei,
      molecule, or chemical system, respectively.

   level of theory
      In quantum chemistry a level of theory is the specification of an
      approximation to the Schroedinger equation. This requires specifying
      the approximate equations (e.g., Hartree-Fock, density functional theory,
      or coupled cluster) and the basis set in which the equations will be
      solved.

   molecular basis set
      An electronic basis set meant to describe a set of electrons interacting
      with multiple nuclei. The molecular basis set is usually just the union
      of the :term:`atomic basis set` on each atom.

   physics model
      We use the term "physics model" to generalize the idea of a
      :term:`level of theory` (which has quantum chemistry connotations) to
      whatever set of equations, parameters, and basis sets are being used to
      compute properties of the :term:`chemical system`.


*************
Abbreviations
*************

.. glossary::

   API
      Stands for application programming interface. This is the interface a
      program exposes so that it can be called by other software.

   DFT
      Stands for density-functional theory. DFT is usually differentiated from
      wavefunction-based theories.

   DSL
      Stands for domain-specific language. A DSL is a series of objects, and
      operations on those objects, which facilitates expressing ideas from a
      specific domain. For example, a quantum chemistry DSL should make it easy
      to build and construct quantum mechanical models of chemistry. Notably
      DSLs are used to express intent, they usually do not actually carry out
      the computation themselves.

   ONIOM
      Stands for "Our own N-layered Integrated molecular Orbital and Molecular
      mechanics". The ONIOM method is a generalization of :term:`QM/MM`, to an
      arbitrary number of methods.

   QM/MM
      Stands for "Quantum Mechanics/Molecular Mechanics". QM/MM is a technique
      where part of a target system is modeled with quantum mechanics and the
      rest is modeled with molecular mechanics.

   UI
      Stands for user interface. This is how end users interact with a software
      component. The UI may be the same or different from the API (how other
      software interacts with the component).
