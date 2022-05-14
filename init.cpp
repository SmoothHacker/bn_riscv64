#include "riscvArch.h"

using namespace BinaryNinja;
extern "C" {
    BN_DECLARE_CORE_ABI_VERSION

    BINARYNINJAPLUGIN bool CorePluginInit() {
        Architecture *riscv = new riscvArch();
        Architecture::Register(riscv);
    }
}