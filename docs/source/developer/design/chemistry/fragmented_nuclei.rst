.. _designing_fragmented_nuclei:

#####################################
Designing the Fragmented Nuclei Class
#####################################

:ref:`designing_fragmented_molecule_class` calls for the ``FragmentedMolecule``
class to be implemented in terms of a ``FragmentedNuclei`` class. This page
describes the design of the ``FragmentedNuclei`` class.

****************************
What is a fragmented nuclei?
****************************

Here, "nuclei" specifically refers to the ``Nuclei`` class. The ``Nuclei``
class holds a set of ``Nucleus`` objects. A ``FragmentedNuclei`` object
holds subsets of a ``Nuclei`` objects. 

*********************************
Why do we need fragmented nuclei?
*********************************

When we break molecules up into fragments, we usually do this by assigning
atoms to fragments (atoms here implying nucleus plus electrons). In practice,
once the atoms have combined to form molecular systems, we get a ``Nuclei``
object and the overall electronic properties (charge and multiplicity) of the
system. 

********************************
Fragmented Nuclei Considerations
********************************

.. _fm_non_disjoint:

Non-disjoint.
   While many initial fragment-based techniques relied on disjoint fragments,
   more modern techniques relax this requirement. The ``FragmentedNuclei``
   class needs to be able to handle multiple fragments, even if those fragments
   are non-disjoint.

.. _fm_container:

Container.
   As described above, the ``FragmentedNuclei`` class is envisioned as being
   filled with subsets of a ``Nuclei`` object. The container should be set-like
   but also support indexing.