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

.. _architecture_of_operator:

##################################
Architecture of Operator Component
##################################

The need for the Operator component stemmed from discussions on the
 :ref:`architecture_of_the_quantum_mechanics_component`.

TODO: Write me!!!

**********************
Operator Consideration
**********************

determinate vs. indeterminate
   Most operators have a determinate form (i.e. a form that contains a set, well
   defined number of particles) and an indeterminate form (i.e., a form written
   in terms of a variable number of particles). Usually we refer to the
   indeterminate form as the "many-particle operator" and the determinate form
   as the "one-electron" (or "two-electron") operator. Generally the
   indeterminate form is written as an indeterminate summation over determinate
   operators.

*****************
Example Use Cases
*****************

The following pseudocode snippet is designed to show some high level examples
of how we envision the operator component to be used. This snippet is used to
motivate the design and may not actually represent the final version in Chemist.

.. code-block:: c++

    // Operators in QM are defined in terms of particles. We start by making
    // some particles. First for the one and two particle operators then for the
    // many-particle operators. Creation of particles is the responsibility of
    // the chemical system component

    Electron e; // Electrons are indistinguishable, no need to make more.
    Nucleus n0 = get_a_nucleus_object();
    Nucleus n1 = get_a_different_nucleus_object();

    ManyElectrons electrons(n_electrons);
    Nuclei nuclei = get_many_nucleus_objects();

    // Creates zero-electron operators
    Kinetic<Nucleus> t_n(n0); // Born-Oppenheimer electronic structure won't use
    Coulomb<Nucleus, Nucleus> v_nn(n0, n1);

    // Create one-electron operators
    Kinetic<Electron> t_e(e);
    Coulomb<Electron, Nucleus> v_enucleus(e, n); // A piece of the next operator
    Coulomb<Electron, Nuclei> v_en(e, nuclei);

    // Create two-electron operator
    // N.b. using the same particle is fine when the particles are wave-like,
    // i.e., they don't have a set position
    Coulomb<Electron, Electron> v_ee(e, e);

    // The above operators allow us to go tensor element by tensor element.
    // That's inefficient, but sometimes desired. In practice, we usually
    // create many-particle operators

    // N.b. for many particles providing the same object twice is allowed and
    // the i != j restriction is understood.
    Coulomb<Nuclei, Nuclei> V_nn(nuclei, nuclei);
    Kinetic<ManyElectron> T_e(electrons);
    Coulomb<ManyElectron, Nuclei> V_en(electrons, nuclei);
    Coulomb<ManyElectron, ManyElectron> V_ee(electrons, electrons);

    // Make the Hamiltonian for the entire system
    Hamiltonian<ManyElectron, Nuclei> H(T_e + V_en + V_ee + V_nn);

    // Extract pieces
    CoreH<ManyElectron, Nuclei> Hcore = H.core();
    ElectronicH<ManyElectron, Nuclei> Helec = H.electronic();

    // Build (restricted) many-electron Fock operator
    Density<ManyElectron> rho = get_one_electron_density();
    J_HF<ManyElectron> J(electrons, rho);
    K_HF<ManyElectron> K(electrons, rho);
    Fock<ManyElectron, Nuclei> F(Hcore + 2.0 * J - K);
