cmake_minimum_required(VERSION 4.2)
project(FEtHEr C)

set(CMAKE_C_STANDARD 11)

add_executable(FEtHEr main.c)

find_package(PkgConfig REQUIRED)
set(PKG_CONFIG_USE_CMAKE_PREFIX_PATH ON)
pkg_check_modules(GTK4 REQUIRED IMPORTED_TARGET gtk4)
target_link_libraries(FEtHEr PRIVATE PkgConfig::GTK4)