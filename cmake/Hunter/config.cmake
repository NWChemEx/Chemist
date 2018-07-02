set(gh_url "https://api.github.com/repos/NWChemEx-Project/Utilities")
set(token "d28e8df2f89944f49e7144c747ffe3a114547469")
hunter_config(
        Utilities
        VERSION 0.0.0
        URL "${gh_url}/tarball/master?access_token=${token}"
        SHA1 "42446742165e643a10778f4a3402a5164728d0bb"
        CMAKE_ARGS "BUILD_TESTS=off"
)
