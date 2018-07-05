set(gh_url "https://api.github.com/repos/NWChemEx-Project/Utilities")
set(token "d28e8df2f89944f49e7144c747ffe3a114547469")
hunter_config(
        Utilities
        VERSION 0.0.0
        URL "${gh_url}/tarball/master?access_token=${token}"
        SHA1 "c3c7d10c77b7744a70f1423e50e0cd039d2064f5"
        CMAKE_ARGS "BUILD_TESTS=off"
)
