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

#include "catch.hpp"
#include <chemist/experimental/point/point.hpp>
#include <cmath>
#include <cstddef>

using namespace chemist::experimental;

namespace chemist_examples {

// Begin as_double
// The point classes deliberately keep the concrete floating-point type out of
// their API; a coordinate comes back as a type-erased value. Whichever type
// you actually want is yours to name, which is what this helper does.
double as_double(wtf::fp::FloatView<const wtf::fp::Float> value) {
    return value.value<double>();
}
// End as_double

// Begin distance
// Because Point and PointView share one API, a function which takes a
// read-only view works on both. A Point converts to one implicitly, so
// callers never write the conversion out.
double distance(const_point_view a, const_point_view b) {
    return as_double((a - b).magnitude());
}
// End distance

// Begin centroid
// The same trick one level up: this takes a read-only view of a set, so it
// can be called with a PointSet, with a mutable view of one, or with a view
// over coordinates chemist does not own.
Point centroid_of(const_point_set_view points) {
    double x = 0.0, y = 0.0, z = 0.0;
    for(auto p : points) {
        x += as_double(p.get_x());
        y += as_double(p.get_y());
        z += as_double(p.get_z());
    }
    const auto n = static_cast<double>(points.size());
    return Point(x / n, y / n, z / n);
}
// End centroid

// Begin external
// Stands in for an external library: it takes bare arrays of coordinates, one
// per Cartesian direction, and knows nothing about chemist.
double nuclear_repulsion(const double* xs, const double* ys, const double* zs,
                         const double* qs, std::size_t n) {
    double rv = 0.0;
    for(std::size_t i = 0; i < n; ++i) {
        for(std::size_t j = i + 1; j < n; ++j) {
            const auto dx = xs[i] - xs[j];
            const auto dy = ys[i] - ys[j];
            const auto dz = zs[i] - zs[j];
            rv += qs[i] * qs[j] / std::sqrt(dx * dx + dy * dy + dz * dz);
        }
    }
    return rv;
}
// End external

int experimental_point_example() {
    // Begin building
    // A water molecule, in atomic units --- the same geometry the AO basis
    // set tutorial uses. The oxygen is first, then the two hydrogens.
    PointSet water{Point(0.0, -0.1432223429807816, 0.0),
                   Point(1.6380335020342418, 1.1365568803584036, 0.0),
                   Point(-1.6380335020342418, 1.1365568803584036, 0.0)};

    REQUIRE(water.size() == 3);
    // End building

    // Begin indexing
    // The set stores each Cartesian direction in its own contiguous array,
    // but it behaves like a container of points. Indexing it gives you
    // something which acts exactly like a Point.
    auto oxygen = water[0];
    REQUIRE(as_double(oxygen.get_y()) == -0.1432223429807816);

    // What you got back is not a copy. It aliases the set, so writing through
    // it writes into the set.
    oxygen.set_z(1.0);
    REQUIRE(as_double(water[0].get_z()) == 1.0);
    oxygen.set_z(0.0);
    // End indexing

    // Begin centering
    // Move the molecule so that its geometric center sits at the origin.
    // Every write goes straight into the set, because every `p` is a view of
    // it rather than a copy of one of its points.
    auto center = centroid_of(water);

    for(auto p : water) {
        p.set_x(as_double(p.get_x()) - as_double(center.get_x()));
        p.set_y(as_double(p.get_y()) - as_double(center.get_y()));
        p.set_z(as_double(p.get_z()) - as_double(center.get_z()));
    }

    // The centroid of the moved molecule is the origin.
    const auto off_center = as_double(centroid_of(water).magnitude());
    REQUIRE(off_center == Catch::Approx(0.0).margin(1.0e-15));
    // End centering

    // Begin measuring
    // The two O-H bond lengths. distance() was written against a view, and
    // water[i] is one, so it can be called directly.
    const auto r1 = distance(water[1], water[0]);
    const auto r2 = distance(water[2], water[0]);
    REQUIRE(r1 == Catch::Approx(r2));
    REQUIRE(r1 == Catch::Approx(2.0786987791109155));

    // The H-O-H angle, from the inner product of the two bond vectors.
    // Subtracting two views gives an owning Point, so the vectors outlive the
    // expression which made them.
    const auto v1 = water[1] - water[0];
    const auto v2 = water[2] - water[0];

    const auto cos_theta =
      as_double(v1.inner_product(v2)) /
      (as_double(v1.magnitude()) * as_double(v2.magnitude()));
    // Water's H-O-H angle, a little under 104 degrees for this geometry.
    const auto degrees = std::acos(cos_theta) * 180.0 / std::acos(-1.0);
    REQUIRE(degrees == Catch::Approx(103.99968755694901));
    // End measuring

    // Begin raw_data
    // An external library wants bare arrays. Because the set is stored as one
    // array per Cartesian direction, there is nothing to repack: ask it for
    // the array you want and hand over the pointer.
    auto xs = water.get_x_buffer();
    auto ys = water.get_y_buffer();
    auto zs = water.get_z_buffer();

    // The buffers are type-erased. Naming `double` here is the consumer's
    // obligation, not chemist's, and the buffer will say no if it is holding
    // something else. Contiguity is worth checking before taking a pointer.
    REQUIRE(xs.is_contiguous());

    auto x_span = xs.value<double>();
    auto y_span = ys.value<double>();
    auto z_span = zs.value<double>();

    // Nuclear charges, in the same order as the points.
    const double charges[] = {8.0, 1.0, 1.0};
    const auto v_nn        = nuclear_repulsion(x_span.data(), y_span.data(),
                                               z_span.data(), charges, water.size());

    // The nuclear repulsion energy of this geometry, in Hartree. Note that it
    // did not change when the molecule was translated.
    REQUIRE(v_nn == Catch::Approx(8.0023669741662));
    // End raw_data

    // Begin views
    // A view of the set is a handle, not a copy. Handing one out lets a caller
    // read or write the original without being able to grow or shrink it.
    point_set_view handle(water);
    handle[0].set_z(0.5);
    REQUIRE(as_double(water[0].get_z()) == 0.5);

    // A read-only view can not be written through at all; `set_z` does not
    // exist on the points it hands out, so the line below would not compile:
    //
    //     const_point_set_view(water)[0].set_z(0.5);
    const_point_set_view read_only(water);
    REQUIRE(read_only.size() == water.size());

    // When you really do want a copy, ask for one.
    auto copy = read_only.as_point_set();
    copy[0].set_z(99.0);
    REQUIRE(as_double(water[0].get_z()) == 0.5);
    // End views

    return 0;
} // experimental_point_example()

} // namespace chemist_examples
