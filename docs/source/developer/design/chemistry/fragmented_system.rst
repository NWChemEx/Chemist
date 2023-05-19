##############################
Designing the FragmentedSystem
##############################

The point of this page is to record the design process of the FragmentedSystem
and affiliated classes.

***************************
What is a FragmentedSystem?
***************************

The ``ChemicalSystem`` (main discussion: :ref:`csd_chemical_system_design`)
represents the physical system we are trying to model. For traditional
electronic structure calculations the ``ChemicalSystem`` is the part of the
input independent from the method/model specification. Many approximate
methods require breaking the target chemical system up into subsets. We term
those subsets fragments and a ``FragmentedSystem`` is a ``ChemicalSystem``
which has been decomposed into fragments.

**********************************
Why do we need a FragmentedSystem?
**********************************

As mentioned, there are a number of approximate methods --- *e.g.*, QM/MM,
fragment based-methods, electronic embedding, and symmetry-adpated perturbation
theory --- which require fragmenting the physical system of interest into a
series of subsystems. The ``FragmentedSystem`` class is the representation of
the fragments.

*******************************
FragmentedSystem Considerations
*******************************

.. _fs_hierarchy:

Hierarchy
   While we have talked about a single ``FragmentedSystem`` class up until this
   point, and given that the ``ChemicalSystem`` class we intend to fragment is 
   itself a hierarchy, it makes sense to parallel the ``ChemicalSystem``
   hierarchy with a "fragmented" hierarchy.

.. _fs_container:

Container
   ``FragmentedSystem`` and the like will be container-based, being a series
   of fragments, each fragment being a view of a ``ChemicalSystem``. 

.. _fs_caps:

Caps
   When fragmenting large covalently bonded systems we often need to sever
   covalent bonds (see :ref:`caps_design`). The fragments of such a system
   will need to be capped.

.. _fs_performance:

Performance
   It is expected that the implementation under the hood will need to be
   somewhat clever in how it stores data and preserves fragment mapping. Of
   note, for fragment-based methods the number of fragments often grows 
   exponetially with system size (many of those fragments need not be
   considered, but that's a different problem). Storing an exponential number
   of copies (even if those copies are only subsets) gets expensive quickly.