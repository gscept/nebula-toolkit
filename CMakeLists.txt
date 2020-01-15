# Nebula Toolkit

# 3.7.2 required for using FindVulkan
cmake_minimum_required(VERSION 3.7.2)

get_filename_component(FIPS_ROOT_DIR "../fips" ABSOLUTE)
include(${FIPS_ROOT_DIR}/cmake/fips.cmake)

option(N_MINIMAL_TOOLKIT "Only minimal toolkit" OFF)
option(N_BUILD_NVTT "use NVTT" ON)

fips_setup(PROJECT nebula-toolkit)
fips_include_directories(.)

fips_add_subdirectory(code)

fips_finish()

