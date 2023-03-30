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

###################
Wavefunction Design
###################


Example Wavefunctions
=====================

- ``SingleReference<ClosedShell<MOSpace<T>>>``. RHF wavefunction.
- ``SingleReference<HighSpinSpace<MOSpace<T>>>``. ROHF wavefunction.
- ``SingleReference<HighSpinSpace<SOSpace<T>>>``. UHF wavefunction.
- ``Reference<GASSpace<SOSpace<T>>>``. GASSCF wavefunction.
- ``Reference<RASSpace<SOSpace<T>>>``. RASSCF wavefunction.
- ``Reference<ActiveSpace<SOSpace<T>>>``. CASSCF wavefunction.
- ``Linear<ClosedShell<MOSpace<T>>>``. (Determinant-based) CI wavefunction.
- ``Exponential<ClosedShell<MOSpace<T>>>``. Coupled cluster wavefunction.
- ``Perturbative<ClosedShell<MOSpace<T>>>``. MPN wavefunction.
- ``Perturbative<FrozenCore<ClosedShell<MOSpace<T>>>>``. Frozen-core MPN.
- ``Perturbative<
