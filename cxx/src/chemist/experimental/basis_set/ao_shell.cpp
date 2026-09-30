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

#include <chemist/experimental/basis_set/ao_shell.hpp>
#include <chemist/experimental/basis_set/ao_shell_view.hpp>

namespace chemist::experimental {

// These two are defined here, rather than inline, because each returns a
// unique_ptr to the other class, and so needs both to be complete.

AOShell::view_pointer AOShell::as_view() const { return as_view_(); }

AOShellView::shell_pointer AOShellView::as_shell() const { return as_shell_(); }

} // namespace chemist::experimental
