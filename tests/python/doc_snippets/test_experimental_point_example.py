# Copyright 2026 NWChemEx-Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import math
import unittest

from chemist.experimental import (
    ImmutablePointSetView,
    Point,
    PointSet,
    PointSetView,
)


# Begin distance
# Point and PointView share one API, so a function which measures a distance
# works on either.
def distance(a, b):
    return (a - b).magnitude()


# End distance


# Begin centroid
# The same one level up: this works on a PointSet, on a view of one, or on
# anything else which behaves like an ordered collection of points.
def centroid_of(points):
    x = y = z = 0.0
    for p in points:
        x += p.get_x()
        y += p.get_y()
        z += p.get_z()
    n = float(len(points))
    return Point(x / n, y / n, z / n)


# End centroid


# Begin external
# Stands in for an external library: it takes plain lists of coordinates, one
# per Cartesian direction, and knows nothing about chemist.
def nuclear_repulsion(xs, ys, zs, qs):
    rv = 0.0
    for i in range(len(xs)):
        for j in range(i + 1, len(xs)):
            dx = xs[i] - xs[j]
            dy = ys[i] - ys[j]
            dz = zs[i] - zs[j]
            rv += qs[i] * qs[j] / math.sqrt(dx * dx + dy * dy + dz * dz)
    return rv


# End external


class TestExperimentalPointExample(unittest.TestCase):
    def test_experimental_point(self):
        # Begin building
        # A water molecule, in atomic units -- the same geometry the AO basis
        # set tutorial uses. The oxygen is first, then the two hydrogens.
        water = PointSet(
            [
                Point(0.0, -0.1432223429807816, 0.0),
                Point(1.6380335020342418, 1.1365568803584036, 0.0),
                Point(-1.6380335020342418, 1.1365568803584036, 0.0),
            ]
        )

        self.assertEqual(len(water), 3)
        # End building

        # Begin indexing
        # The set stores each Cartesian direction in its own array, but it
        # behaves like a container of points. Indexing it gives you something
        # which acts exactly like a Point.
        oxygen = water[0]
        self.assertEqual(oxygen.get_y(), -0.1432223429807816)

        # What you got back is not a copy. It aliases the set, so writing
        # through it writes into the set.
        oxygen.set_z(1.0)
        self.assertEqual(water[0].get_z(), 1.0)
        oxygen.set_z(0.0)
        # End indexing

        # Begin centering
        # Move the molecule so that its geometric center sits at the origin.
        # Every write goes straight into the set, because every `p` is a view
        # of it rather than a copy of one of its points.
        center = centroid_of(water)

        for p in water:
            p.set_x(p.get_x() - center.get_x())
            p.set_y(p.get_y() - center.get_y())
            p.set_z(p.get_z() - center.get_z())

        # The centroid of the moved molecule is the origin.
        self.assertAlmostEqual(centroid_of(water).magnitude(), 0.0, places=14)
        # End centering

        # Begin measuring
        # The two O-H bond lengths. distance() was written against points, and
        # water[i] behaves like one, so it can be called directly.
        r1 = distance(water[1], water[0])
        r2 = distance(water[2], water[0])
        self.assertAlmostEqual(r1, r2)
        self.assertAlmostEqual(r1, 2.0786987791109155)

        # The H-O-H angle, from the inner product of the two bond vectors.
        # Subtracting two views gives an owning Point, so the vectors outlive
        # the expression which made them.
        v1 = water[1] - water[0]
        v2 = water[2] - water[0]

        cos_theta = v1.inner_product(v2) / (v1.magnitude() * v2.magnitude())
        # Water's H-O-H angle, a little under 104 degrees for this geometry.
        degrees = math.degrees(math.acos(cos_theta))
        self.assertAlmostEqual(degrees, 103.99968755694901)
        # End measuring

        # Begin raw_data
        # Hand the coordinates to something which wants plain arrays. Because
        # the set already stores one array per Cartesian direction, there is
        # nothing to repack.
        xs = water.get_x_coordinates()
        ys = water.get_y_coordinates()
        zs = water.get_z_coordinates()

        # Nuclear charges, in the same order as the points.
        charges = [8.0, 1.0, 1.0]
        v_nn = nuclear_repulsion(xs, ys, zs, charges)

        # The nuclear repulsion energy of this geometry, in Hartree. Note that
        # it did not change when the molecule was translated.
        self.assertAlmostEqual(v_nn, 8.0023669741662)
        # End raw_data

        # Begin views
        # A view of the set is a handle, not a copy. Handing one out lets a
        # caller read or write the original without being able to grow or
        # shrink it.
        handle = PointSetView(water)
        handle[0].set_z(0.5)
        self.assertEqual(water[0].get_z(), 0.5)

        # A read-only view can not be written through at all; the points it
        # hands out have no setters.
        read_only = ImmutablePointSetView(water)
        self.assertEqual(len(read_only), len(water))
        self.assertFalse(hasattr(read_only[0], "set_z"))

        # When you really do want a copy, ask for one.
        copy = read_only.as_point_set()
        copy[0].set_z(99.0)
        self.assertEqual(water[0].get_z(), 0.5)
        # End views


if __name__ == "__main__":
    unittest.main(verbosity=2)
