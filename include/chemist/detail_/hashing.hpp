#pragma once
#include <parallelzone/hasher.hpp>

// Functions and types needed for hashing
namespace chemist::detail_ {

// From ParallelZone
using parallelzone::hash_objects;
using parallelzone::hash_to_string;
using parallelzone::Hasher;
using parallelzone::HashType;
using parallelzone::HashValue;
using parallelzone::make_hash;
using parallelzone::make_hasher;

} // namespace chemist::detail_