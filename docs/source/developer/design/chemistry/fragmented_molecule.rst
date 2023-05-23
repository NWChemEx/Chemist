.. _designing_fragmented_molecule_class:

######################################
Designing the FragmentedMolecule Class
######################################

.. |n| replace:: :math:`n`

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

Molecule class.
   Chemist defines the ``Molecule`` class. A set of fragments should function
   like a series of views of the ``Molecule`` class. This has two parts: nuclei
   and the electrons (charge/multiplicity).

.. _fm_charge_mult:

Charge and multiplicity.
   In fragmenting the molecule, the total charge and multiplicity of the
   original molecule must somehow be assigned to the fragments. In both cases, 
   this is usually done by having the user assign the charge, or unpaired 
   electrons, to one or more atoms. When those atoms appear in a fragment,
   the fragment is charged/has unparied electrons too.

.. _fm_non_disjoint:

Non-disjoint.
   While many initial fragment-based techniques relied on disjoint fragments,
   more modern techniques relax this requirement. The ``FragmentedMolecule``
   class needs to be able to handle multiple fragments, even if those fragments
   are non-disjoint.

.. _fm_caps:

Caps.
   Usually the charge/multiplicity of a fragment is neutral/singlet 
   respectively. This only makes sense if one includes caps as part of the 
   fragment (otherwise the fragments would be radicals).

   - Computing caps only requires a FragmentedNuclei object (and the Nuclei)
     object it refers back to.

.. _fm_type_dispatch:

Type dispatch.   
   Modules recieving ``Molecule`` objects as input do not need to worry about
   fragments, whereas modules recieving ``FragmentedMolecule`` objects do need
   to worry about the fact that there is (in general) a set of fragments.


Ignored Considerations
======================

.. |phi_set| replace:: :math:`\left\lbrace\phi_i\right\rbrace`

AO Basis Set.
   It is almost always assumed that if |phi_set| is the atomic basis set
   for atom :math:`i` in the :ref:`supersystem`, then |phi_set| is also the
   atomic basis set for atom :math:`i` in each fragment. In other words, the
   molecular basis set is fragmented in the same way as the :ref:`supersystem`.
   Thus if we know how to generate |phi_set| for any given atom, it is trivial
   to assemble the molecular basis set for a given fragment. This is one reason
   why we do not worry about storing the AO basis set in the 
   ``FragmentedMolecule`` class. Another reason is to parallel the un-fragmented
   hierarchy where ``Molecule`` and ``AOBasisSet`` are separate classes.

NMers.

   Main discussion :ref:`gf_interaction_class_design`.
   
   Many fragment-based methods require forming |n|-mers. Naive,
   applications of the MBE suggest that |n|-mers are "just another system
   we need to compute the energy of", suggesting |n|-mers should be treated 
   similar to fragments. However, more complicated applications (*e.g.*, 
   screened |n|-mers, multi-layer approaches, and BSSE-corrections) tend to
   rely on interactions, which are more complicated than just an |n|-mer (they
   inolve an |n|-mer and usually some overlaps/subsystems, along with linear-
   expansion coefficients). Since interactions are more complicated we feel
   they warrant another class.


Fields.
   While we ultimately want to fragment the chemical system, which includes
   the external fields, we want our fragmented chemical system class to
   parallel the non-fragmented versions. Since the chemical system contains
   a molecule, we want the fragmented chemical system to contain a fragmented
   molecule. The point is that fragmenting the fields is handled elsewhere. 