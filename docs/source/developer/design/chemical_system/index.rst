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

.. _design_of_the_chemical_system_component:

#######################################
Design of the Chemical System Component
#######################################

The chemical system component of Chemist is comprised of classes designed to
represent concepts more familiar to traditional chemists. These concepts
include things like atoms, molecules, chemical bonds, etc. This should be
contrasted with concepts like wavefunctions and operators which are more
familiar to physical chemists

.. note::

   Infrastructure for fragmenting chemical systems is still undergoing heavy
   development and is thus considered experimental.

.. toctree::
   :maxdepth: 2

   architecture
   nucleus
   molecule
   system
