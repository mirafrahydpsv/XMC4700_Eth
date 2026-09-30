#ifndef ETH_PHY_MDIO_H_
#define ETH_PHY_MDIO_H_

#include <stdint.h>
#include <stdbool.h>
#include "test_cases.h"
//#include "eth_phy_mdio.h"
void phy_reset_and_init(unsigned int interface);   /* 'r' = RMII, 'm' = MII */
void mdio_select_input_pin(void);
void ETH_PHY_ConfigureMode(uint8_t phy_addr, bool phy_loopback, uint8_t speed_100m, uint8_t full_duplex);
void mdio_set_clock(unsigned int mdio_clock_frequency);
unsigned int mdio_is_busy();
void mdio_wait_until_idle();
void mdio_select_read_op();
void mdio_select_write_op();
void mdio_set_reg_addr(unsigned int reg_add);
void mdio_set_phy_addr(unsigned int phy_add);
void mdio_set_data_value(unsigned int data);
void eth_select_mii_interface();
void eth_select_rmii_interface();
//void mdio_select_input_pin();
void eth_configure_rmii_clock();
//void phy_reset_and_init(unsigned int interface);
unsigned int mdio_reg_read_raw(unsigned int phy_add, unsigned int reg_add,unsigned int pos,unsigned int mask);
unsigned int mdio_wait_for_transfer();
void mdio_reg_write_raw(unsigned int phy_add, unsigned int reg_add, unsigned int pos,unsigned int mask,unsigned int data);
void mac_apply_config(unsigned char speed,unsigned char duplex,unsigned char f_size);
void eth_configure_dma_bus(unsigned char path, unsigned char priority,unsigned int priority_weights,unsigned int skip_len);

/*------------------------------------PHY (KSZ8081RNA) REGISTER ACCESS-----------------------------------*/
/* PHY address on the MDIO bus - address 0 is what the existing ISR code (ETH0_0_IRQHandler_1) already
 * uses to talk to this board's KSZ8081RNA, so it is kept as the default here. If your board straps the
 * PHY to a different address, change this. */
#define PHY_ADDR                    0x00u

/* IEEE 802.3 clause 22 Basic Control Register - standard on every MII/RMII PHY, KSZ8081RNA included. */
#define PHY_REG_BMCR                0x00u
#define PHY_BMCR_RESET_Msk          0x8000u  /* bit15: Reset                                   */
#define PHY_BMCR_LOOPBACK_Msk       0x4000u  /* bit14: Loopback                                */
#define PHY_BMCR_SPEED100_Msk       0x2000u  /* bit13: Speed select (1 = 100Mbps, 0 = 10Mbps)  */
#define PHY_BMCR_ANEG_EN_Msk        0x1000u  /* bit12: Auto-Negotiation Enable                 */
#define PHY_BMCR_POWERDOWN_Msk      0x0800u  /* bit11: Power down                              */
#define PHY_BMCR_ISOLATE_Msk        0x0400u  /* bit10: Isolate                                 */
#define PHY_BMCR_ANEG_RESTART_Msk   0x0200u  /* bit9:  Restart Auto-Negotiation                */
#define PHY_BMCR_DUPLEX_FULL_Msk    0x0100u  /* bit8:  Duplex mode (1 = full)                  */

/* IEEE 802.3 clause 22 Basic Status Register - link/capability status, read-only. */
#define PHY_REG_BMSR                0x01u
#define PHY_BMSR_ANEG_COMPLETE_Msk  0x0020u  /* bit5: Auto-Negotiation complete                */
#define PHY_BMSR_LINK_STATUS_Msk    0x0004u  /* bit2: Link status (1 = up)                     */

/* PHY Identifier registers - fixed, factory-programmed, read-only. Used purely as a sanity
 * check that MDIO is really talking to a KSZ8081/KSZ8091 at PHY_ADDR: OUI 0010A1h -> PHYID1
 * reads back 0x0022, PHYID2's upper 12 bits read back 0x156 (the low nibble is silicon
 * revision, so PHYID2 will be 0x1560-0x156F depending on die rev - confirmed against the
 * Linux kernel's own KSZ8081 devicetree binding, "ethernet-phy-id0022.1560"). */
#define PHY_REG_ID1                 0x02u
#define PHY_REG_ID2                 0x03u
#define PHY_EXPECTED_ID1            0x0022u
#define PHY_EXPECTED_ID2_Msk        0xFFF0u
#define PHY_EXPECTED_ID2            0x1560u

/* KSZ8081-specific vendor registers. Bit-level decode of the Operation Mode Indication field
 * varies slightly by datasheet revision, so these are read and printed as raw hex rather than
 * decoded here - cross-check against the "PC1R"/"PC2R" tables in the KSZ8081 datasheet if you
 * need the exact speed/duplex/MDI-X breakdown. */
#define PHY_REG_PC1R                0x1Eu  /* vendor: PHY Control 1 - link/operation-mode status */
#define PHY_REG_PC2R                0x1Fu  /* vendor: PHY Control 2 - operation-mode/MDIX config  */

unsigned int phy_read_register(unsigned int phy_add, unsigned int reg_add);
void phy_write_register(unsigned int phy_add, unsigned int reg_add, unsigned int value);
void phy_enable_loopback(unsigned int phy_add);
void phy_disable_loopback(unsigned int phy_add);
void phy_print_all_registers(unsigned int phy_add);
void mdio_write_phy_reg(uint8_t phy_addr, uint8_t reg_addr, uint16_t data);

#endif /* ETH_PHY_MDIO_H_ */

