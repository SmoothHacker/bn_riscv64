cmake_minimum_required(VERSION 3.16)
project (bn_riscv_arch)

set(HEADLESS ON CACHE BOOL "Skip building UI functionality")
add_subdirectory(vendor/binaryninja-api)

set(CMAKE_CXX_STANDARD 17)

set(SOURCE
        init.cpp callingconvention.cpp callingconvention.h lifter.cpp lifter.h disassembler.cpp disassembler.h riscvArch.cpp riscvArch.h)

add_library(bn_riscv_arch SHARED ${SOURCE})
target_link_libraries(bn_riscv_arch binaryninjaapi)
