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

.. _design_of_the_hamiltonian_class:

###############################
Design of the Hamiltonian Class
###############################

These are musings on the design of the Hamiltonian class and should be turned
into full documentation at some point.

- The Hamiltonian will always be an indefinite operator.

    - This is necessary because, in general, it will contain a mix of one- and
      two-particle operators. Bra-kets of such an operator only makes sense with
      many-particle wavefunctions.

- We will want subsets of the terms.

- What should the template parameters, if any, be?

  - On one hand, it would be nice to know ahead of time what terms the
    Hamiltonian has. On the other this will complicate interfaces.
  - Order of parameters can be managed by convention.
  - Is it templated on operators present or particles present?
  - If on particles, how do we know that it say contains not just single
    ``Electron`` terms, but also terms that depend on pairs of ``Electron``
    objects?

.. code-block:: c++

   // These are the proposed templating schemes

   // Templated on operators ("easily" deduced from ctor)
   Hamiltonian<T_e_type, V_en_type, V_ee_type, V_nn_type> H0;

   // Templated on particle types (no distinction of what definite terms appear)
   Hamiltonian<ManyElectrons, Nuclei> H1

   // Templated on definite sizes
   Hamiltonian<ManyElectrons, std::pair<ManyElectrons, ManyElectrons>, ...> H2;

   // Would also be forms for definite particle types

Creation, usage APIs:

.. code-block:: c++

   auto [T_e, V_en, V_ee, V_en, V_nn] = get_operators();

   // Works with C++17, could deduce any of the above types
   Hamiltonian H(T_e + V_en + V_ee + V_en + V_nn);

   // If templated, SFINAE could be used to enable/disable these functions
   T_e = H.T_e(); // Takes an optional offset in case there's more than one

   // Can get commonly needed subsets of terms
   auto H_e = H.H_e(); // Gets the electronic Hamiltonian
   auto H_core = H.H_core(); // Gets core Hamiltonian

   Fock F(H); // Builds the Fock operator that is consistent with H

Defining property types:

.. code-block:: c++

   // Add the terms you want to be able to handle.
   using H_type = Hamiltonian<sort_operators_t<V_en_type, T_e_type, ...>>;

   using pt = EvaluateBraKet<BraKet<Determinant, H_type, Determinant>>;


Problems is what happens when Hamiltonians contain a subset of terms. For
example:

.. code-block:: c++

    using H_type0 = Hamiltonian<sort_operators_t<V_en_type, T_e_type, ...>>;
    using H_type1 = Hamiltonian<sort_operator_t<V_en_type>>;

    using pt0 = EvaluateBraKet<BraKet<Determinant, H_type0, Determinant>>;
    using pt1 = EvaluateBraKet<BraKet<Determinant, H_type1, Determinant>>;

Here Hamiltonians of ``H_type0`` could only be passed to ``pt0`` and
Hamiltonians of ``H_type1`` could only be passed to ``pt1``. More than likely
``pt0`` could handle Hamiltonians of ``H_type1`` (though it is unlikely that
``pt1`` could handle Hamiltonians of ``H_type0``).

Verdict: Go with type-erased to avoid combinatorial interaction of terms.
