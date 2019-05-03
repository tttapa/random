#include "eagle_setup_ipc.hpp"

#include "xil_cache.h"
#include "xil_mmu.h"
#include "xscugic.h"

// https://www.xilinx.com/support/documentation/application_notes/xapp1078-amp-linux-bare-metal.pdf

extern "C" void Xil_L1DCacheFlush(void);

extern u32 MMUTable;

void eagle_setup_ipc(void) { eagle_SetTlbAttributes(0xFFFF0000, 0x04de2); }

void eagle_DCacheFlush(void) {
    Xil_L1DCacheFlush();
    // Xil_L2CacheFlush();
}

void eagle_SetTlbAttributes(u32 addr, u32 attrib) {
    u32 *ptr;
    u32 section;

    mtcp(XREG_CP15_INVAL_UTLB_UNLOCKED, 0);
    dsb();

    mtcp(XREG_CP15_INVAL_BRANCH_ARRAY, 0);
    dsb();
    eagle_DCacheFlush();

    section = addr / 0x100000;
    ptr     = &MMUTable + section;
    *ptr    = (addr & 0xFFF00000) | attrib;
    dsb();
}