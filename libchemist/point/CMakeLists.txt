list(APPEND LIBCHEMIST_HEADERS point/point.hpp
                               point/point_view.hpp
)

list(APPEND LIBCHEMIST_SRCS point/point.cpp)

set(LIBCHEMIST_HEADERS "${LIBCHEMIST_HEADERS}" PARENT_SCOPE)
set(LIBCHEMIST_SRCS "${LIBCHEMIST_SRCS}" PARENT_SCOPE)
