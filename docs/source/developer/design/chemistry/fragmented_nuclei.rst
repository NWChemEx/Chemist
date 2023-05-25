.. _designing_fragmented_nuclei:

#####################################
Designing the Fragmented Nuclei Class
#####################################

********************************
Fragmented Nuclei Considerations
********************************

.. _fm_non_disjoint:

Non-disjoint.
   While many initial fragment-based techniques relied on disjoint fragments,
   more modern techniques relax this requirement. The ``FragmentedNuclei``
   class needs to be able to handle multiple fragments, even if those fragments
   are non-disjoint.