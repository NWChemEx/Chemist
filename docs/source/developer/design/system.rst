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
