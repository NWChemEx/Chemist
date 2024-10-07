###########################
Notes on Adding an Operator
###########################

To add a new operator to Chemist requires modifications in several places. They
are collected here for convenience.

- Forward declare it in 
  include/chemist/quantum_mechanics/operator/operator_fwd.hpp
- Declare the class in 
  include/chemist/quantum_mechanics/operator/operator_name.hpp
- Define the class in src/chemist/quantum_mechanics/operator/operator_name.cpp
- Add it to the OperatorVisitor class in 
  include/chemist/quantum_mechanics/operator/operator_visitor.hpp
- Define the new OperatorVisitor functions in 
  src/chemist/quantum_mechanics/operator/operator_visitor.cpp
- Add unit tests to 
  tests/cxx/unit_tests/quantum_mechanics/operator/operator_name.cpp
- Add unit tests to
  tests/python/unit_tests/quantum_mechanics/operator/operator_name.py
  
.. note::

   "operator_name" in the above file names should be replaced with your
   operator's actual name.

As a helpful hint, copy/paste ``kinetic.hpp`` / ``kinetic.cpp`` for one-
particle operators and ``Coulomb.hpp`` / ``Coulomb.cpp`` for two-particle
operators and then find/replace the name.