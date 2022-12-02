cmake_minimum_required(VERSION 3.16)
project (bn_riscv_arch)

set(HEADLESS ON CACHE BOOL "Skip building UI functionality")
add_subdirectory(vendor/api)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS -g)
set(SOURCE
        src/init.cpp
        src/riscvCallingConvention.cpp
        src/riscvCallingConvention.h
        src/lifter.cpp
        src/lifter.h
        src/disassembler.cpp
        src/disassembler.h
        src/riscvArch.cpp
        src/riscvArch.h)

add_library(bn_riscv_arch SHARED ${SOURCE})
target_link_libraries(bn_riscv_arch binaryninjaapi)

bn_install_plugin(bn_riscv_arch)

# Extra targets ----------------------------------------------------------------
add_custom_target(format
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMAND clang-format -i --style=file
    ${SOURCE})
