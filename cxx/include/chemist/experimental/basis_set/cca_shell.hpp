/*
 * Copyright 2026 NWChemEx-Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

/** @file cca_shell.hpp
 *
 *  Convenience header pulling in the entire experimental AO-shell component,
 *  i.e. the AOShell/AOShellView bases and the CCA-ordered shell which
 *  satisfies them. Include the individual class headers instead if you only
 *  need one of them.
 */

#include <chemist/experimental/basis_set/ao_shell.hpp>
#include <chemist/experimental/basis_set/ao_shell_common.hpp>
#include <chemist/experimental/basis_set/ao_shell_view.hpp>
#include <chemist/experimental/basis_set/cca_shell_class.hpp>
#include <chemist/experimental/basis_set/cca_shell_common.hpp>
#include <chemist/experimental/basis_set/cca_shell_view.hpp>
