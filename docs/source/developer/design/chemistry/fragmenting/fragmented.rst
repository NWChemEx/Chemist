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

This page describes the design of the ``Fragmented`` class.

*****************************
What is the Fragmented Class?
*****************************

The ``Fragmented<T>`` class represents a "superset" ``T`` object which has
been broken into subsets. The elements in the ``Fragmented<T>`` class are
the pieces of the superset, i.e., the fragments.

**********************************
Why Do We Need a Fragmented Class?
**********************************

The overall architecture of the Fragmenting component
(see :ref:`designing_the_fragmenting_component`) requires a ``Fragmented<T>``
class template in order to represent a fragmented ``T`` object. More generally,
we have opted to write a new class, instead of using say a
``std::vector<std::set<T>>`` object, for performance reasons. In particular, we
want to avoid copying members of the superset ``T`` object, which requires some
indirection in the implementation.

*************************
Fragmented Considerations
*************************

.. _ fc_additional_state:

additional state
   The overall design of the Fragmenting component
   (:ref:`designing_the_fragmenting_component`) laid out the container-like
   nature of ``Fragmented<T>`` objects. In practice, some of the objects we are
   fragmenting are not pure containers but have container-wide state. This
   additional state will need to be specially handled by the ``Fragmented<T>``
   class.

   - As a concrete example, the ``Molecule`` class is not simply a container of
     ``Nucleus`` objects, but also knows the total number of electrons and the
     overall multiplicity of the molecular system. While we can track which
     nuclei are in the fragment by offset, we must additionally explicitly store
     the number of electrons and multiplicity of the fragment as well.

.. _fc_non_disjoint2:

non-disjoint
   The discussion on the :ref:`designing_the_fragmenting_component` page
   charged the ``Fragmented<T>`` class with being able to handle non-disjoint
   fragments.

.. _fc_null_and_empty_states:

null and empty states
   The default constructed object should represent a null state, such a
   state does not even have a supersystem set. There is also an empty state
   where the supersystem is set, but the object may not contain any fragments.

   - The superset is part of the state and is involved in all operations.

.. _fc_overall_state:

overall state
   Collecting requirements from :ref:`designing_the_fragmenting_component` and
   the above considerations, the internal state of a ``Fragmented<T>`` class
   must contain:

   - the superset,
   - the members of each fragment,
   - additional state associated with each fragment,
   - and the caps.

*****************
Fragmented Design
*****************

.. _fig_fragmented_class:

.. figure:: assets/fragmented.pn
   :align: center

   Classes related to implementing the ``Fragmented<T>`` class template.

Consideration :ref:`fc_additional_state` means that ``Fragmented<T>`` will need
to contain ``T``-specific state. While specialization is an option,
specialization of a class template requires redefining all members of the class
for every specialization. Since we anticipate needing to do this for every
value of ``T`` we would essentially end up writing independent classes.
However, we anticipate that the implementations for these specializations will
contain some level of replicated code, which we would like to factor out. Our
solution is shown in :ref:`fig_fragmented_class`.

As :ref:`fig_fragmented_class` shows ``Fragmented<T>`` is not specialized,
rather its PIMPL, ``FragmentedPIMPL<T>`` is. This solution allows us to
implement common functionality in the ``Fragmented<T>`` class template in terms
of a common set of functions each specialization of ``FragmentedPIMPL<T>``
must implement. In turn the ``Fragmented<T>`` class relies on static
polymorphism to address the :ref:`fc_additional_state` consideration.
Conceptually, the static polymorphism in this case works similar to duck-typing
in Python, namely the
``Fragmented<T>`` object doesn't really care about the exact signatures of the
PIMPL's functions so long as a function with that name exists and can accept
whatever arguments the user provided.

.. note::

   Full PIMPL design discussions: :ref:`designing_fragmented_nuclei`,
   :ref:`designing_fragmented_molecule_class`, and
   :ref:`designing_fragmented_system`.

While the exact details of the PIMPLs are out of scope for this discussion, we
note that the PIMPL design will be similar to the design of the
``Molecule`` and ``ChemicalSystem`` classes (see :ref:`molecule_design`
and :ref:`csd_chemical_system_design` respectively). More specifically, the
``FragmentedPIMPL<Molecule>`` class will contain a ``Fragmented<Nuclei>``
instance plus the additional ``Molecule`` state for each fragment, and the
``FragmentedPIMPL<ChemicalSystem>`` will contain a ``Fragmented<Molecule>``
instance plus the additional ``ChemicalSystem`` state for each fragment.

**************
Fragmented API
**************

To construct a ``Fragmented<T>`` object:

.. code-block:: C++

   // In this code snippet T is assumed to be Nuclei, Molecule, or
   // ChemicalSystem

   // We somehow make a supersystem object
   T supersys = get_supersystem();

   // Fragmented objects need to know the supersystem the refer to
   // (could also move to avoid the copy)
   Fragmented<T> fragments(supersys);

   // Default construction is null, not empty
   Fragmented<T> null;

Following from consideration :ref:`fc_null_and_empty_states`, ``fragments`` will
be an empty set of fragments whereas ``null`` will be a null object. Because
fragment creation is somewhat involved we do not anticipate users having a set
of fragments ready when creating a ``Fragmented<T>`` object. Rather we expect
fragments to be added dynamically via an algorithm; examples of filling in
fragments were shown in :ref:`fragmenting_api`. Once ``fragments`` is filled
in we expect users to want to be able to access its state:

.. code-block:: C++

   // How many fragments?
   fragments.size();

   // Access the supersystem
   assert(fragments.supersystem() == supersys);

   // Loop over fragments and print them
   for(auto& frag_i : fragments) std::cout << frag_i << std::endl;

*******
Summary
*******

:ref:`fc_additional_state`
   The fact that some supersystems contain state beyond simply being a container
   is handled by specializing ``FragmentedPIMPL<T>`` for each ``T``.

:ref:`fc_non_disjoint2`
   This consideration will primarily apply to ``FragmentedPIMPL<Nuclei>`` which
   is charged with knowing which ``Nucleus`` objects are assigned to which
   fragments. Common functionality implemented in the ``Fragmented<T>`` class
   template will not assume that fragments are disjoint, nor will it enforce
   disjointedness.

:ref:`fc_null_and_empty_states`
   The default constructor will create a null object, whereas the constructor
   which takes a superset creates an empty object.

:ref:`fc_overall_state`
   Storing the state of the ``Fragmented<T>`` object largely falls to the
   various PIMPL classes.
