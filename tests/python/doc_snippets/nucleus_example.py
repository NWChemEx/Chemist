import chemist

# Creating Nucleus objects using different constructors
n0 = chemist.Nucleus()
n1 = chemist.Nucleus("H", 1, 1.0079)
n2 = chemist.Nucleus("H", 1, 1.0079, 0.0, 0.0, 0.0)

# Accessing the properties of a Nucleus object
# Accessing the name
assert n0.name == ""
assert n1.name == "H"
# Accessing the atomic number
assert n0.Z == 0
assert n1.Z == 1
# Accessing the mass
assert n0.mass == 0.0
assert n1.mass == 1.0079
# Accessing the position
assert n0.x == 0.0
assert n0.y == 0.0
assert n0.z == 0.0
assert n1.x == 0.0
# Accessing the charge
assert n0.charge == 0.0
assert n1.charge == 1.0

# Comparing Nucleus objects
assert n0 != n1
assert n1 == n2

# Modifying the properties of a Nucleus object
n1.name = "He"
n1.Z = 2
n1.mass = 4.0026
n1.x = 1.0
n1.y = 2.0
n1.z = 3.0
assert n1 != n2


