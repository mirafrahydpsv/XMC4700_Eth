#include <stdio.h>
#include "XMC4700.h"
#include "eth_registers.h"
#include "reg_access.h"

// The three bit-level operations below (set / clear / toggle) all follow
// the same pattern: read the register, combine it with the mask one way
// or another, write it back. Instead of repeating that read-modify-write
// three times, it's pulled out into one shared helper and each public
// function just tells it which operation to apply.
typedef enum {
    BIT_OP_SET,
    BIT_OP_CLEAR,
    BIT_OP_TOGGLE
} bit_op_t;

static void modify_register(unsigned int addr, unsigned int mask, bit_op_t op) {
    volatile unsigned int *addr_ptr = (volatile unsigned int *)addr;
    unsigned int current = *addr_ptr;
    unsigned int updated;

    switch (op) {
        case BIT_OP_SET:
            updated = current | mask;
            break;
        case BIT_OP_CLEAR:
            updated = current & ~mask;
            break;
        case BIT_OP_TOGGLE:
            updated = current ^ mask;
            break;
        default:
            updated = current;
            break;
    }

    *addr_ptr = updated;
}

void bits_set(unsigned int addr, int pos, unsigned int mask) {
    (void)pos; // kept for call-site readability, not needed for the actual write
    modify_register(addr, mask, BIT_OP_SET);
}

void bits_clear(unsigned int addr, int pos, unsigned int mask) {
    (void)pos;
    modify_register(addr, mask, BIT_OP_CLEAR);
}

void bits_toggle(unsigned int addr, int pos, unsigned int mask) {
    (void)pos;
    modify_register(addr, mask, BIT_OP_TOGGLE);
}

// Write-1-to-clear status registers (e.g. ETH0 STATUS) work differently:
// the bits you write a 1 to get cleared, and everything else should stay
// 0 in the write, so this deliberately does NOT go through
// modify_register()'s read-modify-write - writing back other bits that
// happened to be set at read time would clear them too.
void bits_clear_w1c(unsigned int addr, unsigned int mask) {
    volatile unsigned int *addr_ptr = (volatile unsigned int *)addr;
    *addr_ptr = mask;
}

unsigned int bit_get(unsigned int addr, unsigned int pos) {
    volatile unsigned int *addr_ptr = (volatile unsigned int *)addr;
    return (*addr_ptr >> pos) & 1u;
}

unsigned int reg32_read(unsigned int addr) {
    volatile unsigned int *addr_ptr = (volatile unsigned int *)addr;
    return *addr_ptr;
}

void reg32_write(unsigned int addr, unsigned int data) {
    volatile unsigned int *addr_ptr = (volatile unsigned int *)addr;
    *addr_ptr = data;
}

void field_write(unsigned int addr, unsigned int pos, unsigned int mask, unsigned int data) {
    volatile unsigned int *addr_ptr = (volatile unsigned int *)addr;
    unsigned int cleared = *addr_ptr & ~mask;
    *addr_ptr = cleared | (data << pos);
}

unsigned int field_read(unsigned int addr, unsigned int pos, unsigned int mask) {
    volatile unsigned int *addr_ptr = (volatile unsigned int *)addr;
    return (*addr_ptr & mask) >> pos;
}
