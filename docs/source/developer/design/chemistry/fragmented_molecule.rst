.. _designing_fragmented_molecule_class:

######################################
Designing the FragmentedMolecule Class
######################################

.. |n| replace:: :math:`n`

Following the discussion in :ref:`designing_fragmented_system` we need a class
``FragmentedMolecule`` to track how the ``Molecule`` piece of the 
``ChemicalSystem`` is decomposed. This page discusses the design of that class.

******************************
What is a fragmented molecule?
******************************

In computational chemistry parlance, a molecule is the set of atoms we are
running a computation on. Sometimes we need to describe a "piece of a 
molecule." These pieces are fragments, and when we decompose a molecules into
a set of fragments, we say that the set forms a fragmented molecule if each
atom in the original molecule appears in at least one of the fragments. 

************************************
Why do we need fragmented molecules?
************************************

In computational chemistry there are a number of methods which require breaking
the molecule up into two or more pieces. While at some point many of these
methods will rely on the fact that the fragments are themselves perfectly good
molecules, most times the algorithm will also need to use the fact that the
fragments are pieces of a larger system. In turn, we need a way to represent
the fragments, while not forgetting the molecule the fragments come from.

***************************************
FragmentedMolecule Class Considerations
***************************************

.. _fm_molecule:

Molecule compatability.
   Chemist defines the ``Molecule`` class. The ``FragmentedMolecule`` class
   should behave like a container of ``Molecule`` objects.

.. _fm_charge_mult:

Charge and multiplicity.
   In fragmenting the molecule, the charge/multiplicity must also be fragmented.
   More specifically we must assign a charge/multiplicity to each fragment.

.. _fm_caps:

Caps.
   As a direct result of the :ref:`fs_generality` consideration raised for the
   ``FragmentedSystem`` class, the  ``FragmentedMolecule`` class must be able 
   to handle fragmentation patterns which break covalent-bonds. The usual way
   of dealing with severed bonds is by capping. 

   - Computing caps only requires a FragmentedNuclei object (and the Nuclei)
     object it refers back to.

.. _fm_type_dispatch:

Type dispatch.   
   Modules recieving ``Molecule`` objects as input do not need to worry about
   fragments, whereas modules recieving ``FragmentedMolecule`` objects do need
   to worry about the fact that there is (in general) a set of fragments.

*************************
FragmentedMolecule Design
*************************

The high-level design aspects of the ``FragmentedMolecule`` class were
already covered by :numref:`fig_fragmented_system_design`.

