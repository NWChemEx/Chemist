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

.. _design_of_operator_base:

################################
Design of the OperatorBase Class
################################

These are just some notes of things I thought about when I wrote the class. They
should be formalized at a later point.

- Architecture called for templating on particle type, i.e., 
  ``OperatorBase<T...>``.

  - Problem for Python exposure unless we expose each separately.
  - Would have ``T...`` values of: ``Electron``, ``ManyElectron``, ``Nucleus``,
    ``Nuclei``, ``Density<Electron>``, ``ChargeSet``, and pairs of these.
    That's already 42 possible types (assuming we don't ever need other 
    densities, triples of particles, or more exotic particles like muons).
  - Internally rely on ``std::vector<std::variant>``?

- Provide a class ``OperatorImpl<T...>`` for factoring out the common template
  pieces. ``OperatorImpl<T...>`` derives form ``OperatorBase``. APIs should
  not rely on ``OperatorImpl<T...>``, rather they should rely on 
  ``OperatorBase`` or the class that derives from ``OperatorImpl<T...>``.

- Can't be in namespace ``operator`` (C++ keyword), so we opted for 
  ``qm_operator``.

- Use visitor pattern to implement generic algorithms. Algorithms specific to
  an operator type should go through the derived class.

*********************
Visitor Pattern Notes
*********************

We assume that ``OperatorBase`` defines an API like:

.. code-block:: c++

   class OperatorBase {
   public:
       virtual void visit(OperatorVisitor& visitor) const = 0;
   };

and each operator overloads it like:

.. code-block:: c++

   template<typename T>
   class Kinetic : public OperatorBase {
    public:
       void visit(OperatorVisitor& visitor) const override {
          visitor.run(*this);
       }
   };

The visitor will then be called with the derived class. This requires the 
visitor to define an overload for each operator it might get. The easiest
solution is to brute force enumerate the possible overloads in 
``OperatorVisitor``, implementing them all so that they error. Derived classes
can then override the ones they want to handle. 