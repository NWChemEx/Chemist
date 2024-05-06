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

##################################
Understanding the Traits Component
##################################

.. note::

   TODO\: an actual tutorial, for now the main points are listed.

- Types are often viral because interactions between classes happen through the
  types known to those classes, e.g., for an object of type ``T`` to interact
  with an object of type ``U``, ``T`` must be able to understand the types
  ``U`` uses to access state and vice versa.
- With many Chemist objects actually consisting of a number of different
  classes, we need to synchronize types across those objects.
- Also used for the template meta-programming to ensure const-ness is correct.
- Traits allows a single source of truth for types associated a class.
