cmake_minimum_required(3.10)

find_path(GaiaBlackboards_INCLUDE_DIRS "GaiaBlackboards")
find_library(GaiaBlackboards_LIBS "GaiaBlackboards")

if (GaiaBlackboards_INCLUDE_DIRS AND GaiaBlackboards_LIBS)
    set(GaiaBlackboards_FOUND TRUE)
endif()