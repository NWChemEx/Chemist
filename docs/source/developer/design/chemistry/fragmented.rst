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

.. _designing_fragmented_class:

##############################
Designing the Fragmented Class
##############################

This page describes the design of the ``Fragmented`` class. This discussion
started as part of 
`Chemist PR#361 <https://github.com/NWChemEx-Project/Chemist/pull/361>`_.

*************************
Fragmented Considerations
*************************

superset type
   Ultimately, we're planning on a class template ``Fragmented<T>`` where the
   template parameter ``T`` is type of the superset, i.e., the type of the
   object being fragmented.

   - Conceptually ``T`` will also be the the type of the fragments; however,
     in practice, the fragments may actually be objects of another type ``U`` 
     where ``U`` is a view of ``T``.

.. _fc_superset_state:

superset state
   ``Fragmented<T>`` objects will be containers. The elements of the container
   will be the fragments/subsets. If ``T`` is the type of an object which is
   just a container, then the ``Fragmented<T>`` object can represent membership
   in a fragment by a set of proxies (offsets, pointers, references, etc.) and 
   does not need to physically copy the members into the fragment.

   - In practice, some of the objects we are fragmenting are not pure containers
     but have container-wide state, e.g., ``Molecule`` is not simply a container
     of ``Nucleus`` objects, but also knows the total number of electrons. In
     this case we end up having no choice, but to store the number of electrons
     in each fragment (somewhat of an aside, but note that for charged systems 
     the number of electrons in a fragment is not always easy to compute).
   - Even if the state ultimately is different among choices for ``T`` we 
     anticipate there being common functionality.  

.. _fc_chemistry_specific:

chemistry specific
   The ``Fragmented<T>`` class template is really meant for use with objects
   of type ``T`` where ``T`` is a class defined in Chemist, rather than trying
   to represent more generic superset/subset relationships. The :term:`API` 
   for ``Fragmented`` may thus contain provisions for concepts found when
   creating families of sets for chemistry objects that would not be found when
   taking subsets of more general supersets.

   - Target values of ``T`` are: ``Nuclei``, ``Molecule``, ``ChemicalSystem``,
     and ``AOBasisSet``. 

.. _fc_caps:

caps
  The consideration :ref:`fc_chemistry_specific` originally arose because of the
  need to cap broken bonds. Caps are state that are not part of the superset, 
  but instead belong to the ``Fragmented<T>`` object (the same cap may be 
  needed to cap more than one fragment).

  - A full discussion of cap considerations is beyond the current page (see
    :ref:`designing_the_caps_class`).
  - For the purposes of designing ``Fragmented<T>`` this consideration amounts
    to needing to store caps in the ``Fragmented<T>`` object.


*****************
Fragmented Design
*****************

Consideration :ref:`fc_superset_state` means that we can not simply have one
class template ``Fragmented<T>``, but will have to have specializations of
``Fragmented<T>`` for certain ``T``. Nonetheless, there is still common 
functionality we can capitalize on. Specialization of a class template requires
us to redefine all members of the class, so we want to do the specialization
after factoring out the common functionality. One possibility would be to
adopt a PIMPL approach where the ``Fragmented<T>`` object defines a common API,
and any common implementations it can. The ``FragmentedPIMPL<T>`` class would
then be specialized for each ``T``. The ``FragmentedPIMPL<T>`` objects would
be responsible for creating ``NucleiView``, ``MoleculeView``, etc. for each
fragment.