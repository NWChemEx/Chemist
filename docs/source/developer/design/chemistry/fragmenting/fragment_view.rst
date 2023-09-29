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

.. _designing_the_fragment_view_class:

################################
Designing the FragmentView Class
################################

The point of this page is to document the design decisions which went into
the ``FragmentView<U>`` class template and the classes associated with it.

*******************************
What is the FragmentView Class?
*******************************

Fragments are stored in ``Fragmented<T>`` objects. Each ``FragmentView<U>``
object acts as a reference to a fragment in a ``Fragmented<T>`` object.

************************************
Why Do We Need a FragmentView Class?
************************************

For performance reasons ``Fragmented<T>`` will not just be a
``std::vector<T>``. In turn, obtaining an object which acts like a reference to
an object of type ``T`` is not as easy as accessing an array. Instead we need
to create the reference on-the-fly. Let ``U`` be a type which acts like a
reference to a ``T`` object, then the ``FragmentView<U>`` class will alias the
state inside a ``Fragmented<T>`` object in a manner which allows the state to
be accessed as if it were in a ``U`` object. The need for ``FragmentView<U>``
instead of just a ``U`` object is because fragments in general have additional
state beyond that of the ``U``, e.g., a user may want to know what nuclei, caps
in the fragment replace.

***************************
FragmentView Considerations
***************************

.. _fv_alias_fragmented_state:

alias ``Fragmented<T>`` state
   The primary need for the ``FragmentView<U>`` class is to alias the state in
   a ``Fragmented<T>`` object (actually the ``FragmentedPIMPL<T>`` object
   powering it).

*******************
FragmentView Design
*******************

.. _fig_fragmentview_class:

.. figure:: assets/fragmented_view.png
   :align: center

   Classes related to implementing ``FragmentView<U>``.

Each class we intend to fragment (i.e., ``Nuclei``, ``Molecule``, etc.) has
an associated view class. Let ``U`` be the view class associated with ``T``.
We assume that ``U`` is actually implemented via the PIMPL idiom, and the
class implementing ``U`` is ``UPIMPL``. Furthermore we assume that ``UPIMPL`` is
polymorphic and allows us to derive a class ``FragmentViewPIMPL<UPIMPL>`` which
overrides ``UPIMPL`` so that the ``U`` instance is powered by aliasing state
from a ``Fragmented<T>``. This is shown in :numref:`fig_fragmentview_class`.

For this figure we assume that ``UPIMPL`` will be ``NucleiViewPIMPL``,
``MoleculeViewPIMPL``, and ``ChemicalSystemViewPIMPL`` for
``FragmentView<Nuclei>``, ``FragmentView<Molecule>``, and
``FragmentView<ChemicalSystem>`` respectively. With this assumption we only need
to specialize  ``FragmentViewPIMPL<U>`` for ``NucleiViewPIMPL``. For
``NucleiViewPIMPL`` specialization is needed to ensure that the caps are taken
into account. For the other will have the pointers necessary in the ``FragmentedPIMPL<T>`` object.
