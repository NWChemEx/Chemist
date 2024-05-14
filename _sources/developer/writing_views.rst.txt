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

###################
Creating a New View
###################

.. note::

   TODO\: an actual tutorial, for now the main points are listed.

- Template on the aliased type. For type ``T`` expect ``T`` and ``const T``
- Use traits to work out the correct const-ness of member types
- For template parameter ``T``, ``View<T>`` should support implicit conversions
  from:

  - References to ``T`` objects
  - For ``T == const U``, ``View<U>`` objects

 - Comparisons should work between objects of type ``View<T>`` and all objects
   for which implicit conversions are allowed.
 - Comparison operations need to be defined symmetrically, i.e., make sure the
   view can be on the left and the right side of the operator.
