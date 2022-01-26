#pragma once
#include <parallelzone/hasher.hpp>

// Functions and types needed for hashing
namespace chemist::detail_ {

// From ParallelZone
using pz::hash_objects;
using pz::hash_to_string;
using pz::Hasher;
using pz::HashType;
using pz::HashValue;
using pz::make_hash;
using pz::make_hasher;

} // namespace chemist::detail_