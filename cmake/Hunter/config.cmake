set(gh_url "https://api.github.com/repos/NWChemEx-Project/Utilities")
set(token "d28e8df2f89944f49e7144c747ffe3a114547469")
hunter_config(
        Utilities
        VERSION 0.0.0
        URL "${gh_url}/tarball/master?access_token=${token}"
        SHA1 "e1628322f6d9e30641aca38f1b1bdeab7da8e50c"
        CMAKE_ARGS "BUILD_TESTS=off"
)
