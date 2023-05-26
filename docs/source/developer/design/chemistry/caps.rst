.. _designing_the_caps_class:

########################
Designing the Caps Class
########################

In :ref:`designing_fragmented_molecule_class` consideration :ref:`fm_caps`
gave rise to the need for a ``Caps`` class. This page describes the design of
that object.

**************
What is a cap?
**************

.. |A| replace:: :math:`A`
.. |B| replace:: :math:`B`
.. |AB| replace:: :math:`A-B`

When fragmenting large covalently bonded systems it is usually the case that
there exists at least one bond between say atoms |A| and |B|,
such that there is a fragment which contains |A|, but not |B|.
In this case we say that in forming the fragments we have broken the
|AB| bond. The resulting fragments would be radicals (assuming the
electrons in the bond are split evenly). Treating the fragments as radicals
would lead to substatially different electornic structure, and we instead
usually prefer to instead approximate the severed bond as existing
between |A| and an added monovalent atom (usually hydrogen). The 
monovalent atom (or sometimes group of atoms) is referred to as a "cap".

********************
Why do we need caps?
********************

If we want to apply fragment-based methods to large covalently-bonded systems
then we will necessarily have to break covalent bonds. Since we will not want
to treat the resulting fragments as radicals, we will also need to cap the
fragments. 

*******************
Caps considerations
*******************

.. |C| replace:: :math:`C`
.. |D| replace:: :math:`D`
.. |CB| replace:: :math:`C-B`
.. |AD| replace:: :math:`A-D`
.. |AC| replace:: :math:`A-C`

.. _cc_new_atoms:

New atoms.
   Caps typically are placed in locations in space where no supersystem atom
   resides (they usually are not placed directly where the atom they replace
   resided). This means that the caps will have to be treated as new atoms
   since they are not actuall present in the target supersystem.

.. _cc_asymmetry:

Asymmetry.
   Given a severed bond |AB|, let caps |C| and |D| repsectively replace |A|
   and |B|. It is generally the case that |CB| and |AD| will in general lead
   to dtwo caps because |C| and |D| will not usually reside at the same
   place.

.. _cc_bond_memory:

Bond memory.
   It is important for a cap to remember what atom it replaced and what atom
   it is bonded to.

.. _cc_multiple_caps:

Multiple caps.
   An atom |A| may make bonds to several atoms which are not in the fragment.
   If |A| is bonded to |B| and |C|, and |B| and |C| are both not in the 
   fragment we need caps to address fragmenting the |AB| bond, but also the
   |AC| bond
