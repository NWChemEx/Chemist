#pragma once

namespace libchemist {

/// Class representing a single electron
struct Electron {
    void hash(pluginplay::Hasher&) const noexcept {}
    bool operator==(const Electron&) const noexcept { return true; }
    bool operator!=(const Electron&) const noexcept { return false; }
};

} // namespace libchemist
