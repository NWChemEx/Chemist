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

######################
Chemical System Design
######################

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

Partitioning
   A number of approximate quantum chemistry methods rely on somehow
   decomposing the chemical system into subsystems. While such methods
   typically only consider decompositions of the nuclei, realistically they
   should partition the entire chemical system. In practice, since many of
   the fields are static and interact pair-wise with the molecule, it is often
   the case that the fields for the supersystem end up being also present in
   each subsystem calculation unchanged.


*********************
Future Considerations
*********************

- Could template ``Atom`` on the type of the nucleus and introduce additional
  nucleus types in cases where the nucleus is to be treated quantum
  mechanically.
