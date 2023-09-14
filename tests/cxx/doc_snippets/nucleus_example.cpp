#include <cereal/archives/binary.hpp>
#include <chemist/nucleus/nucleus.hpp>
#include <iostream>
#include <sstream>
#include <string>

using namespace chemist;
namespace chemist_examples {
int nucleus_example() {
    // Creating Nucleus objects using different constructors
    // Default constructor, properties are initialized to:
    // name = '', atomic_number = 0, mass = 0.0
    // x = 0.0, y = 0.0, z = 0.0, charge = 0.0
    Nucleus n0;
    // Constructor with name, atomic number, and mass
    // Coordinates are initialized to: x = 0.0, y = 0.0, z = 0.0
    // Charge is initialized to atomic_number cast to a double, i.e., 1.0
    Nucleus n1("H", 1ul, 1.0079);
    // Constructor with name, atomic number, mass, and x, y, z coordinates
    // Charge is initialized to atomic_number cast to a double, i.e., 1.0
    Nucleus n2("H", 1ul, 1.0079, 0.0, 0.0, 0.0);

    // Accessing the properties of a Nucleus object
    // Accessing the name
    std::string n1_name = n1.name();
    // Accessing the atomic number
    std::size_t n1_Z = n1.Z();
    // Accessing the mass
    double n1_mass = n1.mass();
    // Accessing the coordinates
    double n1_x = n1.x();
    double n1_y = n1.y();
    double n1_z = n1.z();
    // Accessing the charge
    double n1_charge = n1.charge();

    // Comparing Nucleus objects
    // Since all properties are equal, n1_eq_n2 is true
    bool n1_eq_n2 = n1 == n2;

    // Modifying the properties of a Nucleus object
    // We can convert H into He (coolest fusion ever!)
    // Modifying the name
    n1.name() = "He";
    // Modifying the atomic number
    n1.Z() = 2ul;
    // Modifying the mass
    n1.mass() = 4.0026;
    // Modifying the coordinates
    n1.x() = 1.0;
    n1.y() = 2.0;
    n1.z() = 3.0;

    // Printing a Nucleus object
    std::stringstream ss;
    // ss will be: He 1.000000000000000 2.000000000000000 3.000000000000000
    ss << n1;

    // Serializing a Nucleus object
    std::stringstream ss2;
    {
        cereal::BinaryOutputArchive oarchive(ss2);
        oarchive(n1);
    }
    // Deserializing a Nucleus object
    Nucleus n3;
    {
        cereal::BinaryInputArchive iarchive(ss2);
        iarchive(n3);
    }
    return 0;
} // nucleus_example
} // namespace chemist_examples