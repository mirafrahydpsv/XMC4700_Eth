#ifndef REG_ACCESS_H
#define REG_ACCESS_H

// Low-level 32-bit register access helpers used by every ETH/MAC/PHY
// driver file in this project. 'pos' is accepted for readability at the
// call site (it documents which bit the mask starts at) but the actual
// read-modify-write only needs the mask, so pos itself isn't used in the
// implementation.
void bits_set(unsigned int addr, int pos, unsigned int mask);
void bits_clear(unsigned int addr, int pos, unsigned int mask);
void bits_toggle(unsigned int addr, int pos, unsigned int mask);

// For write-1-to-clear status registers (e.g. ETH0 STATUS) where writing
// the raw mask value clears exactly those flags - NOT a read-modify-write.
void bits_clear_w1c(unsigned int addr, unsigned int mask);

unsigned int bit_get(unsigned int addr, unsigned int pos);
unsigned int reg32_read(unsigned int addr);
void reg32_write(unsigned int addr, unsigned int data);
void field_write(unsigned int addr, unsigned int pos, unsigned int mask, unsigned int data);
unsigned int field_read(unsigned int addr, unsigned int pos, unsigned int mask);

#endif
