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

.. _csd_chemical_system_design:

######################
Chemical System Design
######################

The purpose of this page is to collect notes pertaining to the design of the
``ChemicalSystem`` class and its affiliated classes.

.. _csd_what_is_a_chemical_system:

**************************
What is a Chemical System?
**************************

Many quantum chemistry codes talk about the "molecule" as being the input to
the code; however, in chemistry, molecules are simply sets of atoms. Even with
the general definition adopted in :ref:`md_what_is_a_molecule` there are often
additional parts to the system being studied -- for example external fields -- 
that are not captured by the "molecule" concept. We define the chemical system 
to be the entire system we are trying to model. This will almost always 
include a molecule.

*************************************
Chemical System Design Considerations
*************************************

.. _cs_molecule:

Molecule
   Following usual quantum chemistry conventions we define the molecule as all
   the atoms in the system, whether they are covalently bonded or not. In this
   sense a molecule is just a set of atoms. The molecule should describe the
   properties of the nuclei and the electrons comprising it.

.. _cs_periodic:

Periodicity
   Periodic molecules (in the :ref:`cs_molecule` sense of the word) require
   one to specify the molecule (again in the :ref:`cs_molecule` sense) which
   forms the unit cell's contents, as well as the shape of the unit cell.

.. _cs_fields:

Fields
   If all the atoms live in the molecule, there's only a handful of other
   quantities left. We refer to these quantities as fields, using "fields"
   in the physics sense to refer to a function which has a value for every
   point in space. Important sub-considerations:

   - Electric field, from say point charges
   - Magnetic field, from say an NMR experiment
   - Polarizable continuum, from PCM
   - (External) basis-set field, from say ghost atoms.

Out of Scope Considerations
===========================

The considerations in this section were considered in the design, but ultimately
ignored or thought to be better addressed by another component.

Model parameters
   In many cases it is a fine-line between model parameters and the system
   being modeled. Generally speaking we prefer to treat model parameters
   separate from the chemical system when they can be clearly distinguished
   (for example the atomic orbital basis set). 

*********************
Future Considerations
*********************

- Could template ``Atom`` on the type of the nucleus and introduce additional
  nucleus types in cases where the nucleus is to be treated quantum
  mechanically.
