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

###########################
Notes on Adding an Operator
###########################

To add a new operator to Chemist requires modifications in several places. They
are collected here for convenience.

- Forward declare it in 
  include/chemist/quantum_mechanics/operator/operator_fwd.hpp
- Declare the class in 
  include/chemist/quantum_mechanics/operator/<operator_name>.hpp
- Define the class in src/chemist/quantum_mechanics/operator/<operator_name>.cpp
- Add it to the OperatorVisitor class in 
  include/chemist/quantum_mechanics/operator/operator_visitor.hpp
- Define the new OperatorVisitor functions in 
  src/chemist/quantum_mechanics/operator/operator_visitor.cpp
- Add unit tests to 
  tests/cxx/unit_tests/quantum_mechanics/operator/<operator_name>.cpp
- Add unit tests to
  tests/python/unit_tests/quantum_mechanics/operator/<operator_name>.py
  
.. note::

   "<operator_name>" in the above file names should be replaced with your
   operator's actual name.

As a helpful hint, copy/paste ``kinetic.hpp`` / ``kinetic.cpp`` for one-
particle operators and ``Coulomb.hpp`` / ``Coulomb.cpp`` for two-particle
operators and then find/replace the name.