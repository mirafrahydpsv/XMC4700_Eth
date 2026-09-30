#include "XMC4700.h"
#include "eth_registers.h"
#include<stdio.h>
#include "reg_access.h"
#include "eth_mac_cfg.h"
#include <xmc_device.h>
#include <stdint.h>
#include <stdbool.h>
#include "test_cases.h"
#include "eth_mac_cfg.h"

void mac_soft_reset_start();
void mac_soft_reset_in_progress();
void mac_soft_reset_clear();
void mac_set_speed_10m();
void mac_set_speed_100m();
void mac_set_full_duplex();
void mac_set_half_duplex();
void mac_set_normal_frame_size();
void mac_set_jumbo_frame_size();
void mac_set_desc_skip_length(unsigned int skip_len);
unsigned int mac_get_desc_skip_length();
void mac_tx_use_fixed_priority();
void mac_rx_use_fixed_priority();
void mac_set_tx_weights(unsigned int priority_weights);
void mac_set_rx_weights(unsigned int priority_weights);
void mac_load_addr0_low();
void mac_load_addr0_high();
void mac_load_addr1_low();
void mac_load_addr1_high();
void mac_loopback_enable();
void mac_loopback_disable();
unsigned int mac_loopback_is_enabled();

// Enables a software reset of the Ethernet module.
void mac_soft_reset_start(){
    bits_set(ETH0_REG_BUS_MODE_ADDR, ETH_BUS_MODE_SWR_Pos,ETH_BUS_MODE_SWR_Msk);
}

// Reads the status of the software reset bit in the bus mode register.
void mac_soft_reset_in_progress(){
    bit_get(ETH0_REG_BUS_MODE_ADDR,ETH_BUS_MODE_SWR_Pos);
}

void mac_soft_reset_clear(){
    bits_clear(ETH0_REG_BUS_MODE_ADDR, ETH_BUS_MODE_SWR_Pos,ETH_BUS_MODE_SWR_Msk);
}

// Configures the Ethernet speed to 10 Mbps by clearing the FES bit.
void mac_set_speed_10m(){
    bits_clear(ETH0_REG_MAC_CONFIGURATION_ADDR, ETH_MAC_CONFIGURATION_FES_Pos, ETH_MAC_CONFIGURATION_FES_Msk);
}

// Configures the Ethernet speed to 100 Mbps by setting the FES bit.
void mac_set_speed_100m(){
    bits_set(ETH0_REG_MAC_CONFIGURATION_ADDR, ETH_MAC_CONFIGURATION_FES_Pos, ETH_MAC_CONFIGURATION_FES_Msk);
}

// Enables half-duplex mode for Ethernet communication.
void mac_set_half_duplex(){
    bits_clear(ETH0_REG_MAC_CONFIGURATION_ADDR, ETH_MAC_CONFIGURATION_DM_Pos, ETH_MAC_CONFIGURATION_DM_Msk);
}

// Enables full-duplex mode for Ethernet communication.
void mac_set_full_duplex(){
    bits_set(ETH0_REG_MAC_CONFIGURATION_ADDR, ETH_MAC_CONFIGURATION_DM_Pos, ETH_MAC_CONFIGURATION_DM_Msk);
}

// Configures the Ethernet to operate with normal frame size.
void mac_set_normal_frame_size(){
    bits_clear(ETH0_REG_MAC_CONFIGURATION_ADDR,ETH_MAC_CONFIGURATION_JE_Pos,ETH_MAC_CONFIGURATION_JE_Msk);
}

// Configures the Ethernet to operate with jumbo frame size.
void mac_set_jumbo_frame_size(){
    bits_set(ETH0_REG_MAC_CONFIGURATION_ADDR,ETH_MAC_CONFIGURATION_JE_Pos,ETH_MAC_CONFIGURATION_JE_Msk);
}

// Configures the skip length for unchained descriptors.
void mac_set_desc_skip_length(unsigned int skip_len){
//This bit specifies the number of Words to skip between two unchained descriptors. 
//The address skipping starts from the end of current descriptor to the start of next descriptor

	field_write(ETH0_REG_BUS_MODE_ADDR,  ETH_BUS_MODE_DSL_Pos, ETH_BUS_MODE_DSL_Msk , skip_len);
}

// Reads the configured skip length for descriptors.
unsigned int mac_get_desc_skip_length(){
    return field_read(ETH0_REG_BUS_MODE_ADDR,  ETH_BUS_MODE_DSL_Pos, ETH_BUS_MODE_DSL_Msk );
}

// Sets fixed priority for transmit operations.
void mac_tx_use_fixed_priority(){
    bits_set(ETH0_REG_BUS_MODE_ADDR, ETH_BUS_MODE_DA_Pos, ETH_BUS_MODE_DA_Msk); //Priority is given to the tx
    bits_set(ETH0_REG_BUS_MODE_ADDR, ETH_BUS_MODE_TXPR_Pos, ETH_BUS_MODE_TXPR_Msk);
}

// Configures receive fixed priority in the bus mode register.
void mac_rx_use_fixed_priority(){
	bits_set(ETH0_REG_BUS_MODE_ADDR, ETH_BUS_MODE_DA_Pos, ETH_BUS_MODE_DA_Msk);
    bits_clear(ETH0_REG_BUS_MODE_ADDR, ETH_BUS_MODE_TXPR_Pos, ETH_BUS_MODE_TXPR_Msk); //Priority is given to the rx
}

// Configures the Ethernet controller to assign a specific priority weight for transmission.
void mac_set_tx_weights(unsigned int priority_weights){
    bits_clear(ETH0_REG_BUS_MODE_ADDR, ETH_BUS_MODE_DA_Pos, ETH_BUS_MODE_DA_Msk);
    bits_set(ETH0_REG_BUS_MODE_ADDR, ETH_BUS_MODE_TXPR_Pos, ETH_BUS_MODE_TXPR_Msk);
    field_write(ETH0_REG_BUS_MODE_ADDR, ETH_BUS_MODE_PRWG_Pos,ETH_BUS_MODE_PRWG_Msk, priority_weights);
}

// Configures the Ethernet controller to assign a specific priority weight for reception.
void mac_set_rx_weights(unsigned int priority_weights){
    bits_clear(ETH0_REG_BUS_MODE_ADDR, ETH_BUS_MODE_DA_Pos, ETH_BUS_MODE_DA_Msk);
    bits_clear(ETH0_REG_BUS_MODE_ADDR, ETH_BUS_MODE_TXPR_Pos, ETH_BUS_MODE_TXPR_Msk);
    field_write(ETH0_REG_BUS_MODE_ADDR, ETH_BUS_MODE_PRWG_Pos, ETH_BUS_MODE_PRWG_Msk, priority_weights);
}

void mac_load_addr0_low(){
	reg32_write(ETH0_REG_MAC_ADDRESS0_LOW_ADDR, 0xAAAAAAAA);
}

void mac_load_addr0_high(){
	field_write(ETH0_REG_MAC_ADDRESS0_HIGH_ADDR, ETH_MAC_ADDRESS0_HIGH_ADDRHI_Pos, ETH_MAC_ADDRESS0_HIGH_ADDRHI_Msk, 0xAAAA);
}

void mac_load_addr1_low(){
	reg32_write(ETH0_REG_MAC_ADDRESS1_LOW_ADDR, 0xAAAAAAAA);
}

void mac_load_addr1_high(){
	field_write(ETH0_REG_MAC_ADDRESS1_HIGH_ADDR, ETH_MAC_ADDRESS1_HIGH_ADDRHI_Pos,ETH_MAC_ADDRESS1_HIGH_ADDRHI_Msk, 0x8000AAAA);
}

void mac_loopback_enable(){
	bits_set(ETH0_REG_MAC_CONFIGURATION_ADDR, ETH_MAC_CONFIGURATION_LM_Pos, ETH_MAC_CONFIGURATION_LM_Msk);

}

// Clears the MAC's LM (loopback) bit in MAC_CONFIGURATION.
void mac_loopback_disable(){
	bits_clear(ETH0_REG_MAC_CONFIGURATION_ADDR, ETH_MAC_CONFIGURATION_LM_Pos, ETH_MAC_CONFIGURATION_LM_Msk);
}

// Reads back the MAC's LM bit so callers/prints can confirm
unsigned int mac_loopback_is_enabled(){
	return field_read(ETH0_REG_MAC_CONFIGURATION_ADDR, ETH_MAC_CONFIGURATION_LM_Pos, ETH_MAC_CONFIGURATION_LM_Msk);
}

void ETH_MAC_ConfigureMode(bool mac_loopback, uint8_t speed_100m, uint8_t full_duplex)
{
    uint32_t reg_val = reg32_read(ETH0_REG_MAC_CONFIGURATION_ADDR);

    /* 1. MAC Loopback (Bit 12: LM) */
    if (mac_loopback) {
        reg_val |= (1U << 12);
    } else {
        reg_val &= ~(1U << 12);
    }

    /* 2. Speed (Bit 14: FES -> 0: 10M, 1: 100M) */
    if (speed_100m) {
        reg_val |= (1U << 14);
    } else {
        reg_val &= ~(1U << 14);
    }

    /* 3. Duplex (Bit 11: DM -> 0: Half, 1: Full) */
    if (full_duplex) {
        reg_val |= (1U << 11);
    } else {
        reg_val &= ~(1U << 11);
    }

    reg32_write(ETH0_REG_MAC_CONFIGURATION_ADDR, reg_val);
}
