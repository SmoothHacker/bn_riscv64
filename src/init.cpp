#include "riscvArch.h"
#include "riscvCallingConvention.h"

using namespace BinaryNinja;
extern "C" {
BN_DECLARE_CORE_ABI_VERSION

BINARYNINJAPLUGIN bool CorePluginInit() {
    Architecture *riscv = new riscvArch("RISC-V", LittleEndian);
    Architecture::Register(riscv);

    // Calling Convention
    auto *riscvCallConv = new riscvCallingConvention(riscv);
    riscv->RegisterCallingConvention(riscvCallConv);
    riscv->SetDefaultCallingConvention(riscvCallConv);

#define EM_RISCV 243
    BinaryViewType::RegisterArchitecture(
            "ELF",
            EM_RISCV,
            BigEndian,
            riscv
    );

    BinaryViewType::RegisterArchitecture(
            "ELF",
            EM_RISCV,
            LittleEndian,
            riscv
    );
    return true;
}
}