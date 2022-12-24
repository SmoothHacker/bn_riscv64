# BN_RISCV_Arch

A C++ architecture plugin for RISC-V 64I.

## Get Started
Simply clone the repository and the API submodule

```sh
git clone --recursive git@github.com:SmoothHacker/bn_riscv64.git
```

## Building

Simply configure the project using CMake, then build and install to your plugins folder:

```sh
cmake -S . -B build # -GNinja ...
cmake --build build -j $(nproc) -t install
```

## TODO
 * Add Support for the following extensions
    * Multiplication and Division
    * Single-Precision Floating-Point
    * Double-Precision Floating-Point