from chemist.basis_set import AOBasisSetD, AtomicBasisSetD, ShellD, ContractedGaussianD
from chemist import PointD, ShellType


def h2_coords():
    return [0.0, 0.0, 0.0, 0.0, 0.0, 1.3983972315]


def h2_basis():
    coords = h2_coords()
    h0_coords = PointD(coords[0], coords[1], coords[2])
    h1_coords = PointD(coords[3], coords[4], coords[5])

    coefs = [0.1543289673, 0.5353281423, 0.4446345422]
    exps = [3.425250914, 0.6239137298, 0.1688554040]
    h0_cg = ContractedGaussianD(coefs, exps, h0_coords)
    h1_cg = ContractedGaussianD(coefs, exps, h1_coords)

    cartesian = ShellType.cartesian
    h0 = AtomicBasisSetD('STO-3G', 1, h0_coords)
    h0.add_shell(cartesian, 0, h0_cg)
    h1 = AtomicBasisSetD('STO-3G', 1, h1_coords)
    h1.add_shell(cartesian, 0, h1_cg)

    rv = AOBasisSetD()
    rv.add_center(h0)
    rv.add_center(h1)

    return rv
