/*
 * Copyright 2022 NWChemEx-Project
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
#include "../detail_/container_pimpl.hpp"
#include "chemist/molecule/molecule.hpp"

namespace chemist::detail_ {

class MolPIMPL : public ContainerPIMPL<Molecule> {
private:
    /// Container hold the atoms of this Molecule
    std::vector<value_type> m_atms_;

    /// Implements the clone() method
    std::unique_ptr<ContainerPIMPL<Molecule>> clone_() const override {
        return std::make_unique<MolPIMPL>(*this);
    }

    /// Implements the at() method
    reference at_(size_type i) override { return m_atms_[i]; }

    /// Implements the size() method
    size_type size_() const noexcept override { return m_atms_.size(); }

    /// Implements the push_back() method
    void push_back_(value_type atom) override {
        m_atms_.push_back(std::move(atom));
    }

    /// Implements the begin() method
    iterator begin_() noexcept override { return m_atms_.begin(); }
    const_iterator begin_() const noexcept override { return m_atms_.cbegin(); }

    /// Implements the end() method
    iterator end_() noexcept override { return m_atms_.end(); }
    const_iterator end_() const noexcept override { return m_atms_.cend(); }
};

} // namespace chemist::detail_
