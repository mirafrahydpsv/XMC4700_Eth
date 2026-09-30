#include<stdio.h>
#include "reg_access.h"
#include "eth_registers.h"
#include "XMC4700.h"
#include "eth_phy_mdio.h"
#include "eth_mac_cfg.h"
#define aircr 0xE000ED0C

void eth_power_on();
void eth_power_off();
void eth_clk_enable();
void eth_clk_disable();
unsigned int eth_clk_is_enabled();
void eth_hold_in_reset();
void eth_release_from_reset();
unsigned int eth_is_in_reset();
void eth_power_clock_init();

/*---------------------------------------------POWER ENABLE MODE-------------------------------------------------*/

// Brings the Ethernet MAC out of power-down mode by clearing the PWRDWN bit.
// Function to put Ethernet MAC into active mode
void eth_power_on() {

    // Clear the PWRDWN bit (bit 0) to bring MAC out of power-down mode
    bits_clear(ETH0_REG_PMT_CONTROL_STATUS_ADDR, ETH_PMT_CONTROL_STATUS_PWRDWN_Pos, ETH_PMT_CONTROL_STATUS_PWRDWN_Msk);
}

/*--------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------POWER DISABLE MODE-------------------------------------------------*/

// Puts the Ethernet MAC into power-down mode by setting the PWRDWN bit.
// Function to put Ethernet MAC into power-down mode
void eth_power_off() {

    // Clear the PWRDWN bit (bit 0) to bring MAC out of power-down mode
    bits_set(ETH0_REG_PMT_CONTROL_STATUS_ADDR, ETH_PMT_CONTROL_STATUS_PWRDWN_Pos, ETH_PMT_CONTROL_STATUS_PWRDWN_Msk);
}

/*--------------------------------------------------------------------------------------------------------*/

/*----------------------------------------ENABLE ETH CLOCK-----------------------------------------------*/

// Enables the Ethernet clock by setting the ETH0CEN bit in the clock set register.
// Function to enable Ethernet clock by setting the ETH0CEN bit
void eth_clk_enable() {
    bits_set(SCU_REG_CLKSET_ADDR, SCU_CLK_CLKSET_ETH0CEN_Pos, SCU_CLK_CLKSET_ETH0CEN_Msk);  // Set bit 2 for ETH0CEN
}

/*--------------------------------------------------------------------------------------------------------*/

/*------------------------------------DISABLE ETH CLOCK-----------------------------------------------*/

// Disables the Ethernet clock by setting the ETH0CDI bit in the clock clear register.
void eth_clk_disable(){
    bits_set(SCU_REG_CLKCLR_ADDR, SCU_CLK_CLKCLR_ETH0CDI_Pos, SCU_CLK_CLKCLR_ETH0CDI_Msk);
}

/*--------------------------------------------------------------------------------------------------------*/

/*------------------------------------ETH CLOCK STATUS-----------------------------------------------*/

// Reads the Ethernet clock status bit to determine if the clock is enabled or disabled.
unsigned int eth_clk_is_enabled(){
    return bit_get(SCU_REG_CLKSTAT_ADDR, SCU_CLK_CLKSTAT_ETH0CST_Pos);
}

/*--------------------------------------------------------------------------------------------------------*/

/*----------------------------------------ENABLE ETH RESET-----------------------------------------------*/

// Asserts the Ethernet reset signal by setting the ETH0RS bit in the reset register.
// Function to enable Ethernet clock by setting the ETH0CEN bit
void eth_hold_in_reset() {
    bits_set(SCU_REG_PRSET2_ADDR, SCU_RESET_PRSET2_ETH0RS_Pos, SCU_RESET_PRSET2_ETH0RS_Msk);
}

/*--------------------------------------------------------------------------------------------------------*/

/*------------------------------------DISABLE ETH RESET-----------------------------------------------*/

// Deasserts the Ethernet reset signal by clearing the ETH0RS bit in the reset clear...
void eth_release_from_reset(){
    bits_set(SCU_REG_PRCLR2_ADDR, SCU_RESET_PRCLR2_ETH0RS_Pos, SCU_RESET_PRCLR2_ETH0RS_Msk);
}

/*--------------------------------------------------------------------------------------------------------*/

/*------------------------------------ETH RESET STATUS-----------------------------------------------*/

// Reads the Ethernet reset status bit to determine if the reset is active or not.
unsigned int eth_is_in_reset(){
    return bit_get(SCU_REG_PRSTAT2_ADDR, SCU_RESET_PRSTAT2_ETH0RS_Pos);
}

/*--------------------------------------------------------------------------------------------------------*/

/*------------------------------------ETH CONFIGURE CLOCK POWER RESET-------------------------------------*/

// Configures Ethernet by enabling power, clock, and reset in sequence.
void eth_power_clock_init(){
    eth_clk_enable();
    bits_set(SCU_REG_CGATSET2_ADDR,SCU_CLK_CGATSET2_ETH0_Pos,SCU_CLK_CGATSET2_ETH0_Msk);
    bits_set(SCU_REG_CGATCLR2_ADDR,SCU_CLK_CGATCLR2_ETH0_Pos,SCU_CLK_CGATCLR2_ETH0_Msk);
    eth_release_from_reset();

    mdio_set_clock(72);

}

void cpu_system_reset(){
	field_write(aircr,PPB_AIRCR_VECTKEY_Pos,PPB_AIRCR_VECTKEY_Msk,0x5FB);
	bits_clear(aircr,PPB_AIRCR_VECTRESET_Pos,PPB_AIRCR_VECTRESET_Msk);
	bits_clear(aircr,PPB_AIRCR_VECTCLRACTIVE_Pos,PPB_AIRCR_VECTCLRACTIVE_Msk);
	bits_set(aircr,PPB_AIRCR_SYSRESETREQ_Pos,PPB_AIRCR_SYSRESETREQ_Msk);
}
/*--------------------------------------------------------------------------------------------------------*/


void time_stamp_init(){
/* Timestamp block kept from the original init sequence (unrelated to the
 * loopback test itself, but needed for the rest of the peripheral to be
 * in a sane state). */
field_write(ETH0_REG_TARGET_TIME_SECONDS_ADDR,0,0xFFFFFFFF,0X1F);
field_write(ETH0_REG_TARGET_TIME_NANOSECONDS_ADDR,0,0xFFFFFFFF,0X1F);
bits_set(ETH0_REG_TIMESTAMP_CONTROL_ADDR,0,0x1);
bits_set(ETH0_REG_TIMESTAMP_CONTROL_ADDR,8,0x100);

field_write(ETH0_REG_SYSTEM_TIME_SECONDS_UPDATE_ADDR,0,0xFFFFFFFF,0Xf);
field_write(ETH0_REG_SYSTEM_TIME_NANOSECONDS_UPDATE_ADDR,0,0xFFFFFFFF,0Xf);
bits_set(ETH0_REG_TIMESTAMP_CONTROL_ADDR,2,0x4);
bits_set(ETH0_REG_TIMESTAMP_CONTROL_ADDR,3,0x8);
bits_set(ETH0_REG_TIMESTAMP_CONTROL_ADDR,4,0x10);
}
