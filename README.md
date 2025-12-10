<!--
  ~ Copyright 2022 NWChemEx-Project
  ~
  ~ Licensed under the Apache License, Version 2.0 (the "License");
  ~ you may not use this file except in compliance with the License.
  ~ You may obtain a copy of the License at
  ~
  ~ http://www.apache.org/licenses/LICENSE-2.0
  ~
  ~ Unless required by applicable law or agreed to in writing, software
  ~ distributed under the License is distributed on an "AS IS" BASIS,
  ~ WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  ~ See the License for the specific language governing permissions and
  ~ limitations under the License.
-->

![Chemist Logo](docs/source/assets/logo.png)

[![Nightly Workflow](https://github.com/NWChemEx/Chemist/actions/workflows/nightly.yaml/badge.svg)](https://github.com/NWChemEx/Chemist/actions/workflows/nightly.yaml)

[Documentation](https://nwchemex.github.io/Chemist)

Chemist provides chemistry-related data structures. This includes data
structures for chemistry concepts including atoms, molecules, and atomic-orbital
basis sets.

# Features

Chemist was designed as part of the NWChemEx stack, which means the data
structures in Chemist are designed to be both user-friendly and performant.
Despite being part of the NWChemEx stack, Chemist is a fairly modular library
and can easily be leveraged by other electronic structure packages as well. At
present the main features of Chemist are:

- Support of both structure-of-array and array-of-structures usage.
- Ability to avoid copying via "views".
- The ability to independently control how the nuclei, atoms, molecules, and
  overall system are modeled.
- Efficient C++ classes with Python bindings.

# Installation

Chemist ultimately uses CMake for configuring and building. This means that
installation is usually achieved via a variation on:

```.sh
git clone https://github.com/NWChemEx/Chemist
cd Chemist
cmake -H. -Bbuild -D...
cmake --build build --target install
```
More detailed install instructions can be found
[here](https://nwchemex.github.io/Chemist/install.html).

# Contributing

- [Contributor Guidelines](https://github.com/NWChemEx/.github/blob/1a883d64519f62da7c8ba2b28aabda7c6f196b2c/.github/CONTRIBUTING.md)
- [Contributor License Agreement](https://github.com/NWChemEx/.github/blob/master/.github/CONTRIBUTING.md#contributor-license-agreement-cla)
- [Developer Documentation](https://nwchemex.github.io/Chemist/developer/index.html)
- [Code of Conduct](https://github.com/NWChemEx/.github/blob/master/.github/CODE_OF_CONDUCT.md)

# Acknowledgments

This research was supported by the Exascale Computing Project (17-SC-20-SC), a collaborative effort of the U.S. Department of Energy Office of Science and the National Nuclear Security Administration.
