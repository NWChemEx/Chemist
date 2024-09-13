.. Copyright 2024 NWChemEx-Project
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

#########################
Design of the VectorSpace
#########################

Some notes:

- Having a common base class is at odds with inheriting from
  ``IndexableContainerBase``. We'd have to template ``VectorSpace`` on the
  derived class to have ``IndexableContainerBase`` implement it.

  - CRTP for ``VectorSpace`` is fine if we never need a ``VectorSpace`` object.
    Would we want a property type like:
    ``BraKet<VectorSpace, Operator, VectorSpace>`` vs say
    ``BraKet<AOs, Operator, AOs>``? The former is useful for avoiding a
    combinatorial explosion in property types, e.g.
    ``BraKet<MOs, Operator, AOs>`` and ``BraKet<AOs, Operator, MOs>``.
