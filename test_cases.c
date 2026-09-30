/*
 * test_cases.c
 *
 *  Created on: Aug 18, 2026
 *      Author: PSVLAB0
 */
#include <stdio.h>
#include "XMC4700.h"
#include "test_cases.h"
#include "eth_mac_cfg.h"
#include "eth_phy_mdio.h"
#include "eth_registers.h"
#include "reg_access.h"
#include "eth_tx_dma.h"
#include "eth_rx_dma.h"
#include "debug_uart.h"
#include <string.h>

/* run_mac_loopback_test() / run_phy_loopback_test() are implemented in main.c
 * and are not declared in any shared header, so they are declared extern here. */
extern int run_mac_loopback_test(ETH_TX_FRAME* Frame, ETH_RX_FRAME* rx_Frame,
                                  ETH_TX_DESC* tx_desc_list, ETH_RX_DESC* rx_desc_list);
extern int run_phy_loopback_test(ETH_TX_FRAME* Frame, ETH_RX_FRAME* rx_Frame,
                                  ETH_TX_DESC* tx_desc_list, ETH_RX_DESC* rx_desc_list);
static void delay_ms(uint32_t ms)
{
    for (volatile uint32_t i = 0; i < (ms * 18000); i++) {
        __NOP();
    }
}

unsigned int internal_loopback(ETH_TX_FRAME* Frame, ETH_RX_FRAME* rx_Frame,
                                ETH_TX_DESC* tx_desc_list, ETH_RX_DESC* rx_desc_list){
	printf("\n\n\n\n===== Ethernet MAC internal (LM bit) loopback test =====\n");

	unsigned int passed = 0;
	unsigned int failed = 0;

	for(unsigned int run = 1; run <= LOOPBACK_TEST_RUNS; run++){
		printf("\n--- run %u/%u ---\n", run, LOOPBACK_TEST_RUNS);
		if(run_mac_loopback_test(Frame, rx_Frame, tx_desc_list, rx_desc_list)){
			passed++;
		}
		else{
			failed++;
		}
	}
	printf("\n===== SUMMARY (MAC-only loopback): %u/%u runs passed =====\n", passed, LOOPBACK_TEST_RUNS);
	return failed;
}

unsigned int phy_loopback(ETH_TX_FRAME* Frame, ETH_RX_FRAME* rx_Frame,
                           ETH_TX_DESC* tx_desc_list, ETH_RX_DESC* rx_desc_list){
	printf("\n\n\n\n\n===== PHY (KSZ8081RNA) loopback test - BMCR bit14 =====\n");

	unsigned int phy_passed = 0;
	unsigned int phy_failed = 0;

	for(unsigned int run = 1; run <= LOOPBACK_TEST_RUNS; run++){
		printf("\n--- run %u/%u ---\n", run, LOOPBACK_TEST_RUNS);
		if(run_phy_loopback_test(Frame, rx_Frame, tx_desc_list, rx_desc_list)){
			phy_passed++;
		}
		else{
			phy_failed++;
		}
	}
	printf("\n===== SUMMARY (PHY loopback): %u/%u runs passed =====\n", phy_passed, LOOPBACK_TEST_RUNS);
	return phy_failed;
}


/* ------------------------------------------------------------------------------------------
 * Bring the Ethernet MAC + DMA back to a known-good state before every test case.
 *
 * Why this exists: previously nothing was reset between tests, so
 *   (a) STATUS flags (UNF/TPS/TU/RU/RPS...) from one failed test stayed set, and the TX DMA
 *       was left stuck in "waiting for status" (STATUS.TS = 2) - every later test then timed
 *       out with the same STATUS value, and
 *   (b) MAC_CONFIGURATION.FES / DM / LM were changed while TE and RE (transmitter/receiver)
 *       were still enabled. Changing speed/duplex on a running MAC glitches the TX path and
 *       gives a TX underflow, which is what happened as soon as a 100 Mbps test ran.
 * A software reset (BUS_MODE.SWR) clears the DMA/MTL/MAC state machines and all registers, so
 * we reset, apply speed/duplex/loopback with TE/RE off, and only then re-enable TX/RX.
 * Returns true on success, false if the reset never completed (no RMII REF_CLK).
 * ---------------------------------------------------------------------------------------- */
static bool eth_test_reset_and_configure(bool mac_loopback, uint8_t speed_100m, uint8_t full_duplex)
{
    /* 1. Stop everything that is running. */
    bits_clear(ETH0_REG_MAC_CONFIGURATION_ADDR, ETH_MAC_CONFIGURATION_TE_Pos, ETH_MAC_CONFIGURATION_TE_Msk);
    bits_clear(ETH0_REG_MAC_CONFIGURATION_ADDR, ETH_MAC_CONFIGURATION_RE_Pos, ETH_MAC_CONFIGURATION_RE_Msk);
    tx_dma_stop();
    rx_dma_stop();

    /* 2. Software reset of the whole ETH core. SWR only clears when the RMII clock is present. */
    mac_soft_reset_start();
    uint32_t timeout = 1000000u;
    while (bit_get(ETH0_REG_BUS_MODE_ADDR, ETH_BUS_MODE_SWR_Pos) && --timeout) {
        __NOP();
    }
    if (timeout == 0u) {
        printf("[DEBUG] MAC software reset did not complete (no RMII REF_CLK?)\r\n");
        return false;
    }

    /* 3. Clear any stale STATUS flags (write-1-to-clear). */
    bits_clear_w1c(ETH0_REG_STATUS_ADDR, 0xFFFFFFFFu);

    /* 4. Re-apply the setup main() does once at boot, since SWR put registers back to defaults. */
    mdio_set_clock(72);
    bits_set(ETH0_REG_MAC_CONFIGURATION_ADDR, 7, 0x80);

    /* 5. Speed / duplex / loopback while TE and RE are still OFF. */
    if (mac_loopback) {
        mac_loopback_enable();
    } else {
        mac_loopback_disable();
    }
    mac_apply_config(speed_100m ? 'h' : 'l', full_duplex ? 'f' : 'h', 'n');

    /* 6. Now enable the DMA bus config and interrupts; this also sets TE and RE. */
    eth_configure_dma_bus('r', 'r', 3, 0x0);
    return true;
}



bool ETH_RunTestCase(eth_test_id_t test_id,
                     ETH_TX_FRAME* Frame,
                     ETH_RX_FRAME* rx_Frame,
                     ETH_TX_DESC* tx_desc_list,
                     ETH_RX_DESC* rx_desc_list)
{
    bool is_mac_lb = false;
    bool is_phy_lb = false;
    uint8_t speed_100m = 0;
    uint8_t full_duplex = 0;

    switch (test_id) {
        case ETH_TEST_MAC_10M_HALF:  is_mac_lb = true;  speed_100m = 0; full_duplex = 0; break;
        case ETH_TEST_MAC_10M_FULL:  is_mac_lb = true;  speed_100m = 0; full_duplex = 1; break;
        case ETH_TEST_MAC_100M_HALF: is_mac_lb = true;  speed_100m = 1; full_duplex = 0; break;
        case ETH_TEST_MAC_100M_FULL: is_mac_lb = true;  speed_100m = 1; full_duplex = 1; break;

        case ETH_TEST_PHY_10M_HALF:  is_phy_lb = true;  speed_100m = 0; full_duplex = 0; break;
        case ETH_TEST_PHY_10M_FULL:  is_phy_lb = true;  speed_100m = 0; full_duplex = 1; break;
        case ETH_TEST_PHY_100M_HALF: is_phy_lb = true;  speed_100m = 1; full_duplex = 0; break;
        case ETH_TEST_PHY_100M_FULL: is_phy_lb = true;  speed_100m = 1; full_duplex = 1; break;
        default: return false;
    }

    /* 1. Stop DMA TX & RX temporarily */
    uint32_t op_mode = reg32_read(ETH0_REG_OPERATION_MODE_ADDR);
    reg32_write(ETH0_REG_OPERATION_MODE_ADDR, op_mode & ~(0x00002002));

    /* 2. Configure External PHY via MDIO (Register 0: BMCR) */
    uint16_t bmcr = 0;
    if (is_phy_lb)   bmcr |= (1U << 14); // Loopback
    if (speed_100m)  bmcr |= (1U << 13); // Speed: 100M
    if (full_duplex) bmcr |= (1U << 8);  // Full Duplex
    mdio_write_phy_reg(PHY_ADDR, 0x00, bmcr);

    /* 3. Configure MAC Register (LM, FES, DM, DO, DCRS) */
    uint32_t mac_cfg = reg32_read(ETH0_REG_MAC_CONFIGURATION_ADDR);
    mac_cfg |= (1U << 2) | (1U << 3);  // RE=1, TE=1
    mac_cfg |= (1U << 15);             // PS=1 (Port Select: 10/100 MII/RMII)

    // MAC Loopback bit 12
    if (is_mac_lb) {
        mac_cfg |= (1U << 12);
    } else {
        mac_cfg &= ~(1U << 12);
    }

    // Speed bit 14 (FES)
    if (speed_100m) {
        mac_cfg |= (1U << 14);
    } else {
        mac_cfg &= ~(1U << 14);
    }

    // Duplex bit 11 (DM), Receive Own bit 13 (DO), Carrier Sense bit 16 (DCRS)
    if (full_duplex) {
        mac_cfg |= (1U << 11);         // DM = 1 (Full Duplex)
        mac_cfg &= ~(1U << 16);        // DCRS = 0
    } else {
        mac_cfg &= ~(1U << 11);        // DM = 0 (Half Duplex)
        mac_cfg &= ~(1U << 13);        // DO = 0 (Receive Own ENABLED in Half Duplex)
        mac_cfg |= (1U << 16);         // DCRS = 1 (Ignore carrier sense to allow loopback)
    }
    reg32_write(ETH0_REG_MAC_CONFIGURATION_ADDR, mac_cfg);

    /* Allow PHY PLL and RMII clock to stabilize (essential for 100 Mbps) */
    delay_ms(80);

    /* 4. Re-arm All Descriptors in the 192-byte chain (3 descriptors x 64B) */
    ETH_RX_DESC* rx_desc = rx_desc_list;
    ETH_TX_DESC* tx_desc = tx_desc_list;
    for (int d = 0; d < 3; d++) {
        // Clear RX buffers and grant ownership to DMA
        rx_desc->RDES0 = 0x80000000;
        rx_desc->RDES1 = 0x81000040;

        // Grant TX ownership to DMA
        if (d == 0) {
            tx_desc->TDES0 = 0xB0080000; // First segment
        } else if (d == 2) {
            tx_desc->TDES0 = 0xD0080000; // Last segment
        } else {
            tx_desc->TDES0 = 0x90080000; // Middle segment
        }
        tx_desc->TDES1 = 0x01000040;

        rx_desc = (ETH_RX_DESC*)rx_desc->RDES3;
        tx_desc = (ETH_TX_DESC*)tx_desc->TDES3;
    }

    /* 5. Reset Descriptor Head Pointers */
    reg32_write(ETH0_REG_TRANSMIT_DESCRIPTOR_LIST_ADDRESS_ADDR, (uint32_t)tx_desc_list);
    reg32_write(ETH0_REG_RECEIVE_DESCRIPTOR_LIST_ADDRESS_ADDR, (uint32_t)rx_desc_list);

    /* Clear pending status flags */
    reg32_write(ETH0_REG_STATUS_ADDR, 0xFFFFFFFF);

    /* 6. Enable Store-and-Forward (TSF=1, RSF=1) to prevent Underflow at 100 Mbps */
    op_mode |= (1U << 21) | (1U << 25); // TSF = 1, RSF = 1
    op_mode |= (1U << 20);              // FTF = 1 (Flush TX FIFO)
    op_mode |= (1U << 13) | (1U << 1);  // ST = 1, SR = 1
    reg32_write(ETH0_REG_OPERATION_MODE_ADDR, op_mode);

    /* Wake up DMA controllers */
    reg32_write(ETH0_REG_RECEIVE_POLL_DEMAND_ADDR, 1);
    reg32_write(ETH0_REG_TRANSMIT_POLL_DEMAND_ADDR, 1);

    /* 7. Wait for TX Completion */
    uint32_t tx_timeout = 200000;
    while ((tx_desc_list->TDES0 & 0x80000000) && --tx_timeout) {
        __NOP();
    }
    if (tx_timeout == 0) {
        printf("[DEBUG] Timeout: TX incomplete (STATUS=0x%08X)\r\n",
               (unsigned int)reg32_read(ETH0_REG_STATUS_ADDR));
        return false;
    }

    /* 8. Wait for RX Completion */
    uint32_t rx_timeout = 200000;
    while ((rx_desc_list->RDES0 & 0x80000000) && --rx_timeout) {
        __NOP();
    }
    if (rx_timeout == 0) {
        printf("[DEBUG] Timeout: Packet not received by DMA (STATUS=0x%08X)\r\n",
               (unsigned int)reg32_read(ETH0_REG_STATUS_ADDR));
        return false;
    }

    /* 9. Verify Payload */
    if (memcmp(Frame->tx_buff, rx_Frame->rx_buff, 64) == 0) {
        printf("TEST CASE: PASS - 192 bytes verified through ETH_RunTestCase loopback\r\n");
        return true;
    } else {
        printf("[DEBUG] Payload mismatch!\r\n");
        return false;
    }
}
