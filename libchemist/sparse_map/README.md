Sparse Map Library
==================

The sparse map component of LibChemist focuses on creating, using, and applying
spars maps.

Design
------

### Domain

At the guts of the `Domain` class is the `DomainPIMPL` class. This class is
responsible for holding the contents of the `Domain` and giving it back through
the agreed upon API. The reality is there are many different ways that a
`DomainPIMPL` may hold the indices (e.g. hold them individually, hold them as a
range, or hold them as ordinals) which impact the look-up times and memory
footprints, but not fundamentally what indices are in the `Domain`. The intent
is that the `DomainPIMPL` class hierarchy hides these details.

The next class is the DomainBase class. This class implements
`Domain<TileIndex>` and `Domain<ElementIndex>` via CRTP. Basically the two
template instantiations of `Domain` work more-or-less the same way so to avoid
copy/pasting code between the specializations we define a common base class that
implements the shared machinery.
