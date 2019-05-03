#pragma once

#include <cstdint>  // uint32_t

void eagle_setup_ipc(void);
void eagle_DCacheFlush(void);
void eagle_SetTlbAttributes(uint32_t addr, uint32_t attrib);