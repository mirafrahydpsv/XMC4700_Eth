#include<stdio.h>
#include "reg_access.h"
#include "eth_registers.h"
#include "XMC4700.h"
#include "eth_mac_cfg.h"
#include "eth_tx_dma.h"
#include "eth_rx_dma.h"
#include "eth_power_clock.h"
#include "eth_phy_mdio.h"   /* pulls in PHY_ADDR / PHY_REG_BMCR / PHY_BMCR_*_Msk used by phy_loopback_enable()/disable() below */

#define PHY_BMCR_REG             (0x00U)
#define PHY_BMCR_LOOPBACK_BIT    (1U << 14)
#define PHY_BMCR_SPEED_100M_BIT  (1U << 13)
#define PHY_BMCR_AUTONEG_EN_BIT  (1U << 12)
#define PHY_BMCR_DUPLEX_FULL_BIT (1U << 8)


void mdio_set_clock(unsigned int mdio_clock_frequency);
unsigned int  mdio_is_busy();
void mdio_select_read_op();
void mdio_select_write_op();
void mdio_set_reg_addr(unsigned int reg_add);
void mdio_phy_address(unsigned int phy_add);
void mdio_set_data_value(unsigned int data);
void eth_select_mii_interface();
void eth_select_rmii_interface();
void mdio_select_input_pin();
void eth_configure_rmii_clock();
void phy_reset_and_init(unsigned int interface);
unsigned int mdio_reg_read_raw(unsigned int phy_add, unsigned int reg_add,unsigned int pos,unsigned int mask);
void mdio_reg_write_raw(unsigned int phy_add, unsigned int reg_add, unsigned int pos,unsigned int mask,unsigned int data);
void mac_apply_config(unsigned char speed,unsigned char duplex,unsigned char f_size);
void eth_configure_dma_bus(unsigned char path, unsigned char priority,unsigned int priority_weights,unsigned int skip_len);

/*------------------------------------MDIO CONFIGURE CLOCK-----------------------------------------------*/

// Performs an MDIO write operation to send data to a specified PHY register.
// Function to configure the MDC clock range in the GMII_ADDRESS register
void mdio_set_clock(unsigned int mdio_clock_frequency) {
    unsigned int clock_range = 0;

    // Determine clock range based on the given ETH clock frequency
    if (mdio_clock_frequency >= 60 && mdio_clock_frequency <= 100) {
        clock_range = 0x0; // ETH Clock /42
    } else if (mdio_clock_frequency > 100 && mdio_clock_frequency <= 150) {
        clock_range = 0x1; // ETH Clock /62
    } else if (mdio_clock_frequency >= 20 && mdio_clock_frequency < 35) {
        clock_range = 0x2; // ETH Clock /16
    } else if (mdio_clock_frequency >= 35 && mdio_clock_frequency < 60) {
        clock_range = 0x3; // ETH Clock /26
    } else if (mdio_clock_frequency > 150 && mdio_clock_frequency <= 250) {
        clock_range = 0x4; // ETH Clock /102
    } else if (mdio_clock_frequency > 250 && mdio_clock_frequency <= 300) {
        clock_range = 0x5; // ETH Clock /124
    } else {
        printf("Invalid ETH Clock Frequency\n");
        clock_range = 0x0;
    }

    // Write the clock range value to bits [5:2]
    field_write(ETH0_REG_GMII_ADDRESS_ADDR , ETH_GMII_ADDRESS_CR_Pos, ETH_GMII_ADDRESS_CR_Msk, clock_range);
}

/*--------------------------------------------------------------------------------------------------------*/

/*------------------------------------MDIO PREAPARE FRAME-----------------------------------------------*/

// Reads the MII busy bit to check the status of the ongoing MDIO operation.
unsigned int mdio_is_busy() {
    return bit_get(ETH0_REG_GMII_ADDRESS_ADDR, ETH_GMII_ADDRESS_MB_Pos);
}

// Sets the MII busy bit to start an MDIO operation.
void mdio_wait_until_idle() {
    bits_set(ETH0_REG_GMII_ADDRESS_ADDR, ETH_GMII_ADDRESS_MB_Pos, ETH_GMII_ADDRESS_MB_Msk);
}

// Clears the MW (MII Write) bit to indicate a read operation.
void mdio_select_read_op(){
    bits_clear(ETH0_REG_GMII_ADDRESS_ADDR, ETH_GMII_ADDRESS_MW_Pos, ETH_GMII_ADDRESS_MW_Msk);
}

// Sets the MW (MII Write) bit to indicate a write operation.
void mdio_select_write_op(){
    bits_set (ETH0_REG_GMII_ADDRESS_ADDR, ETH_GMII_ADDRESS_MW_Pos, ETH_GMII_ADDRESS_MW_Msk);
}

// Configures the MII register address for MDIO operation.
void mdio_set_reg_addr(unsigned int reg_add){
    field_write(ETH0_REG_GMII_ADDRESS_ADDR, ETH_GMII_ADDRESS_MR_Pos, ETH_GMII_ADDRESS_MR_Msk, reg_add);
}

// Configures the PHY address for MDIO operation.
void mdio_set_phy_addr(unsigned int phy_add){
    field_write(ETH0_REG_GMII_ADDRESS_ADDR, ETH_GMII_ADDRESS_PA_Pos,ETH_GMII_ADDRESS_PA_Msk, phy_add);
}

// Writes the data value to the MDIO data register.
void mdio_set_data_value(unsigned int data){
    field_write(ETH0_REG_GMII_DATA_ADDR, ETH_GMII_DATA_MD_Pos, ETH_GMII_DATA_MD_Msk, data);
}
/*--------------------------------------------------------------------------------------------------------*/

/*------------------------------------MDIO CONFIGURATION-----------------------------------------------*/

// Configures the MAC interface to MII mode.
void eth_select_mii_interface(){
    bits_clear(SCU_REG_CON_ADDR, ETH_CON_INFSEL_Pos,ETH_CON_INFSEL_Msk);
}

// Configures the MAC interface to RMII mode.
void eth_select_rmii_interface(){
    bits_set(SCU_REG_CON_ADDR, ETH_CON_INFSEL_Pos,ETH_CON_INFSEL_Msk);
}

// Selects the MDIO input for communication.
void mdio_select_input_pin(){
    field_write(SCU_REG_CON_ADDR, ETH_CON_MDIO_Pos, ETH_CON_MDIO_Msk, 0x01);
}

// Configures the MDIO interface pins for operation.
void eth_configure_rmii_clock(){
    
    field_write(SCU_REG_CON_ADDR, ETH_CON_RXD0_Pos, ETH_CON_RXD0_Msk, 0x00);
    field_write(SCU_REG_CON_ADDR, ETH_CON_RXD1_Pos, ETH_CON_RXD1_Msk, 0x00);
    field_write(SCU_REG_CON_ADDR, ETH_CON_CLK_RMII_Pos, ETH_CON_CLK_RMII_Msk, 0x02);
    field_write(SCU_REG_CON_ADDR, ETH_CON_CRS_DV_Pos, ETH_CON_CRS_DV_Msk,0x02);
    field_write(SCU_REG_CON_ADDR, ETH_CON_RXER_Pos, ETH_CON_RXER_Msk, 0x00);
    field_write(SCU_REG_CON_ADDR, ETH_CON_CLK_TX_Pos, ETH_CON_CLK_TX_Msk, 0x03);

}

/*--------------------------------------------------------------------------------------------------------*/

void phy_reset_and_init(unsigned int interface){
    if(interface == 'r'){
    	eth_configure_rmii_clock();
    }
    else if(interface == 'm'){
    	eth_select_mii_interface();
    }

}

/*------------------------------------------------MDIO READ-----------------------------------------------*/

// Performs an MDIO read operation to fetch data from a specified PHY register.
unsigned int mdio_reg_read_raw(unsigned int phy_add, unsigned int reg_add,unsigned int pos,unsigned int mask){

    mdio_set_reg_addr(reg_add);
    mdio_set_phy_addr(phy_add);

    while (mdio_is_busy() & 0x1) {
          // Wait for the Busy bit to be cleared
    }
    mdio_select_read_op();
    mdio_wait_until_idle();
//    unsigned int reg_val = read_reg(reg_add);

    return 1;

}

unsigned int mdio_wait_for_transfer(){

	      return  field_read(ETH0_REG_GMII_DATA_ADDR,0, 0xFFFF);
}

/*--------------------------------------------------------------------------------------------------------*/

/*------------------------------------------------MDIO WRITE-----------------------------------------------*/

// Performs an MDIO write operation to send data to a specified PHY register.
void mdio_reg_write_raw (unsigned int phy_add, unsigned int reg_add, unsigned int pos,unsigned int mask,unsigned int data){

    mdio_set_reg_addr(reg_add);
    mdio_set_phy_addr(phy_add);

    field_write(ETH0_REG_GMII_DATA_ADDR, pos, mask, data);
    mdio_select_write_op();

    while (mdio_is_busy() & 0x1) {
          // Wait for the Busy bit to be cleared
    }

    mdio_wait_until_idle();

}

/*--------------------------------------------------------------------------------------------------------*/

/*------------------------------------PHY (KSZ8081RNA) REGISTER ACCESS-----------------------------------*/

// Full 16-bit MDIO read of a PHY register, returning the actual
unsigned int phy_read_register(unsigned int phy_add, unsigned int reg_add){
    mdio_reg_read_raw(phy_add, reg_add, 0, 0xFFFF);
    return mdio_wait_for_transfer();
}

// Full 16-bit MDIO write to a PHY register. Always writes all
void phy_write_register(unsigned int phy_add, unsigned int reg_add, unsigned int value){
    mdio_reg_write_raw(phy_add, reg_add, 0, 0xFFFF, value & 0xFFFFu);
}

// Puts the PHY itself into loopback via bit 14 of the standard
void phy_enable_loopback(unsigned int phy_add){
    unsigned int bmcr = 0x0000u;
    bmcr |= PHY_BMCR_LOOPBACK_Msk;      /* bit14: loopback                    */
    bmcr |= PHY_BMCR_DUPLEX_FULL_Msk;   /* bit8:  full duplex                 */
    /* bit13 left 0 -> 10Mbps, bit12 left 0 -> auto-negotiation disabled */
    phy_write_register(phy_add, PHY_REG_BMCR, bmcr);
}

// Clears BMCR bit 14 to take the PHY back out of loopback,
void phy_disable_loopback(unsigned int phy_add){
    unsigned int bmcr = phy_read_register(phy_add, PHY_REG_BMCR);
    bmcr &= ~PHY_BMCR_LOOPBACK_Msk;
    phy_write_register(phy_add, PHY_REG_BMCR, bmcr);
}

// Reads back and prints BMCR, BMSR, the PHY ID registers, and
void phy_print_all_registers(unsigned int phy_add){
    unsigned int bmcr = phy_read_register(phy_add, PHY_REG_BMCR);
    unsigned int bmsr = phy_read_register(phy_add, PHY_REG_BMSR);
    unsigned int id1  = phy_read_register(phy_add, PHY_REG_ID1);
    unsigned int id2  = phy_read_register(phy_add, PHY_REG_ID2);
    unsigned int pc1r = phy_read_register(phy_add, PHY_REG_PC1R);
    unsigned int pc2r = phy_read_register(phy_add, PHY_REG_PC2R);

    printf("  ----- PHY register dump (phy_add=%u) -----\n", phy_add);

    printf("    BMCR  (0x00) = 0x%04X  [loopback=%u aneg_en=%u speed100=%u full_duplex=%u powerdown=%u isolate=%u]\n",
        bmcr,
        (bmcr & PHY_BMCR_LOOPBACK_Msk)    ? 1u : 0u,
        (bmcr & PHY_BMCR_ANEG_EN_Msk)     ? 1u : 0u,
        (bmcr & PHY_BMCR_SPEED100_Msk)    ? 1u : 0u,
        (bmcr & PHY_BMCR_DUPLEX_FULL_Msk) ? 1u : 0u,
        (bmcr & PHY_BMCR_POWERDOWN_Msk)   ? 1u : 0u,
        (bmcr & PHY_BMCR_ISOLATE_Msk)     ? 1u : 0u);

    printf("    BMSR  (0x01) = 0x%04X  [link_up=%u aneg_complete=%u]\n",
        bmsr,
        (bmsr & PHY_BMSR_LINK_STATUS_Msk)   ? 1u : 0u,
        (bmsr & PHY_BMSR_ANEG_COMPLETE_Msk) ? 1u : 0u);

    printf("    PHYID1(0x02) = 0x%04X, PHYID2(0x03) = 0x%04X", id1, id2);
    if((id1 == 0xFFFFu && id2 == 0xFFFFu) || (id1 == 0x0000u && id2 == 0x0000u)){
        printf("  -> NO RESPONSE on MDIO (all-1s or all-0s). Check PHY_ADDR, MDC/MDIO wiring, PHY power/reset - nothing else read from this PHY can be trusted until this is fixed\n");
    }
    else if(id1 == PHY_EXPECTED_ID1 && (id2 & PHY_EXPECTED_ID2_Msk) == PHY_EXPECTED_ID2){
        printf("  -> matches KSZ8081/KSZ8091 (OUI 0010A1h) - confirms MDIO is really reaching this PHY\n");
    }
    else{
        printf("  -> does NOT match the expected KSZ8081/KSZ8091 ID (0022h / 1560h-156Fh) - check PHY_ADDR\n");
    }

    printf("    PC1R(0x1E, vendor) = 0x%04X, PC2R(0x1F, vendor) = 0x%04X (raw - see KSZ8081 datasheet for Operation Mode Indication decode)\n",
        pc1r, pc2r);
}

/*--------------------------------------------------------------------------------------------------------*/

void mac_apply_config(unsigned char speed,unsigned char duplex,unsigned char f_size){

	if(f_size == 'n'){
		        mac_set_normal_frame_size(); //n defines the normal frame
		    }
		    else if(f_size == 'j'){
		        mac_set_jumbo_frame_size(); //j defines the jumbo frame
		    }
	   if(speed == 'h'){   //h defines high - 100mbps
	        mac_set_speed_100m();
	    }
	    else if(speed == 'l'){ //l defines low speed - 10mbps
	        mac_set_speed_10m();
	    }
	    if(duplex == 'f'){
	        mac_set_full_duplex(); //f defines the full duplex mode
	    }
	    else if(duplex == 'h'){
	        mac_set_half_duplex();
	    }

	    mac_load_addr0_low();
	    mac_load_addr0_high();
	    mac_load_addr1_low();
	    mac_load_addr1_high();

}

void eth_configure_dma_bus(unsigned char path, unsigned char priority,unsigned int priority_weights,unsigned int skip_len){
	if(path == 't' && priority == 'f'){
		mac_tx_use_fixed_priority();
	}
	else if(path == 'r' && priority == 'f'){
		mac_rx_use_fixed_priority();
	}
	else if(path == 't' && priority == 'r'){
		mac_set_tx_weights(priority_weights);
	}
	else if(path == 'r' && priority == 'r'){
		mac_set_rx_weights(priority_weights);
	}
	bits_set(ETH0_REG_BUS_MODE_ADDR,ETH_BUS_MODE_AAL_Pos,ETH_BUS_MODE_AAL_Msk);
	bits_set(ETH0_REG_BUS_MODE_ADDR,ETH_BUS_MODE_USP_Pos,ETH_BUS_MODE_USP_Msk);
	field_write(ETH0_REG_BUS_MODE_ADDR,ETH_BUS_MODE_RPBL_Pos,ETH_BUS_MODE_RPBL_Msk, 4);
	field_write(ETH0_REG_BUS_MODE_ADDR,ETH_BUS_MODE_PBL_Pos,ETH_BUS_MODE_PBL_Msk, 4);
	mac_set_desc_skip_length(skip_len);
	tx_enable_dma_interrupts();
    rx_enable_dma_interrupts();
}

void ETH_PHY_ConfigureMode(uint8_t phy_addr, bool phy_loopback, uint8_t speed_100m, uint8_t full_duplex)
{
    uint16_t bmcr_val = 0;

    if (phy_loopback) {
        bmcr_val |= PHY_BMCR_LOOPBACK_BIT;
    }
    if (speed_100m) {
        bmcr_val |= PHY_BMCR_SPEED_100M_BIT;
    }
    if (full_duplex) {
        bmcr_val |= PHY_BMCR_DUPLEX_FULL_BIT;
    }

    /* Disable Auto-Negotiation to force fixed speed/duplex */
    bmcr_val &= ~PHY_BMCR_AUTONEG_EN_BIT;

    mdio_write_phy_reg(phy_addr, PHY_BMCR_REG, bmcr_val);
}

// Simple wrapper used by ETH_PHY_ConfigureMode() and the test cases: full 16-bit write of one PHY register.
void mdio_write_phy_reg(uint8_t phy_addr, uint8_t reg_addr, uint16_t data)
{
    phy_write_register(phy_addr, reg_addr, data);
}
