cmake_minimum_required(VERSION 3.16)
project (bn_riscv_arch)

set(HEADLESS ON CACHE BOOL "Skip building UI functionality")
add_subdirectory(vendor/binaryninja-api)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS -g)
set(SOURCE
        init.cpp riscvCallingConvention.cpp riscvCallingConvention.h lifter.cpp lifter.h disassembler.cpp disassembler.h riscvArch.cpp riscvArch.h)

add_library(bn_riscv_arch SHARED ${SOURCE})
target_link_libraries(bn_riscv_arch binaryninjaapi)

bn_install_plugin(bn_riscv_arch)
