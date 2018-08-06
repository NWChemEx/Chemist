set(gh_url "https://api.github.com/repos/NWChemEx-Project")
set(token "d28e8df2f89944f49e7144c747ffe3a114547469")
nwx_depend(
        NAME Utilities
        VERSION 0.0.0
        URL "${gh_url}/Utilities/tarball/master?access_token=${token}"
        SKIP_SHA1
        CMAKE_ARGS "BUILD_TESTS=off"
)

hunter_config(
        bphash
        VERSION 1.1
        URL "https://github.com/bennybp/BPHash/archive/v1.1.tar.gz"
        SHA1 "88e14eda8b1f8224caeae8c43407770a651e027e"
        CMAKE_ARGS "BUILD_SHARED_LIBS=True"
)

nwx_depend(
    NAME SDE
    VERSION 0.0.0
    URL "${gh_url}/SDE/tarball/master?access_token=${token}"
    SKIP_SHA1
    CMAKE_ARGS "BUILD_TESTS=off;BUILD_PYBINDINGS=FALSE"
)

nwx_depend(
        NAME TAMM
        VERSION 0.0.0
        URL "${gh_url}/TAMM/tarball/alloc?access_token=${token}"
        SKIP_SHA1
        CMAKE_ARGS "CMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}"
)
