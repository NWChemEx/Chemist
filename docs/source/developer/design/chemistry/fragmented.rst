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
   Ultimately, we're planning on a class template ``Fragmented<T>`` which is
   templated on the type of the object being fragmented, i.e., the superset.

.. _fc_superset_state:

superset state
   Each subset of the superset will contain a subset of the superset's state. To
   the extent that each superset is a container, the ``Fragmented<T>`` object
   would just need to hold offsets (or similar identifier) to the members of
   the subset.

   - In practice, some of the objects we are fragmenting are not pure containers
     but have container-wide state, e.g., ``Molecule`` is not simply a container
     of ``Nucleus`` objects, but also knows the total number of electrons.
   - Even if the state is different among choices for ``T`` we anticipate there
     being common functionality.  

.. _fc_chemistry_specific:

chemistry specific
   The ``Fragmented`` class is really targeted at representing subset/superset
   relationships among objects in Chemist's chemistry component. The :term:`API` 
   for ``Fragmented`` may thus contain provisions for concepts found when
   creating families of sets for chemistry objects that would not be found when
   taking subsets of more general supersets.

   - This consideration is specifically arising because of caps, which appear as 
     additional state caused by severing covalent bonds.
   - Target values of ``T`` are: ``Nuclei``, ``Molecule``, ``ChemicalSystem``,
     and ``AOBasisSet``. Each of these systems are affected by adding a cap.  



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