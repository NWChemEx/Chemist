set(gh_url "https://api.github.com/repos/NWChemEx-Project/Utilities")
set(token "a869897b4a98f43589ca5af1cc2b599f973b8619")
nwx_depend(
        NAME Utilities
        VERSION 0.0.0
        URL "${gh_url}/tarball/master?access_token=${token}"
        SKIP_SHA1
        CMAKE_ARGS "BUILD_TESTS=off"
)
