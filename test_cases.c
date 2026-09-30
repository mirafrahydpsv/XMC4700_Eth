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

    unsigned int frame_size = (unsigned int)sizeof(ETH_TX_FRAME);

    /* Start from a clean slate: known TX/RX buffers and empty descriptors. */
    memset(Frame, 0x00, sizeof(ETH_TX_FRAME));
    memset(rx_Frame, 0xFF, sizeof(ETH_RX_FRAME));   /* junk, so a PASS has to be real */
    memset(tx_desc_list, 0, sizeof(ETH_TX_DESC) * 4);
    memset(rx_desc_list, 0, sizeof(ETH_RX_DESC) * 4);

    /* 1. Configure PHY (BMCR: loopback / speed / duplex, auto-negotiation off) */
    ETH_PHY_ConfigureMode(PHY_ADDR, is_phy_lb, speed_100m, full_duplex);

    /* 2. Configure MAC (LM loopback bit, speed, duplex, MAC addresses, frame size) */
    if (is_mac_lb) {
        mac_loopback_enable();
    } else {
        mac_loopback_disable();
    }
    mac_apply_config(speed_100m ? 'h' : 'l', full_duplex ? 'f' : 'h', 'n');

    /* Let the PHY / clocks settle before the first frame. */
    for (volatile uint32_t d = 0; d < 150000u; d++) {
        __NOP();
    }

    /* 3. Arm the RX DMA first so it is ready before anything is sent. */
    rx_set_store_forward_mode();
    rx_dma_stop();
    rx_set_desc_list_base(rx_desc_list);
    rx_desc_build_ring(rx_desc_list, (unsigned int*)rx_Frame, frame_size);
    rx_dma_start();

    /* 4. Prepare the TX frame and kick off the TX DMA. */
    tx_fill_test_frame(Frame);
    tx_set_store_forward_mode();
    tx_dma_stop();
    tx_set_desc_list_base(tx_desc_list);
    tx_desc_mark_first_seg(tx_desc_list);
    tx_desc_build_ring(tx_desc_list, (unsigned int*)Frame, frame_size);
    tx_dma_poll_demand();
    tx_dma_start();

    /* 5. Wait for TX and RX to complete (both are bounded, so this cannot hang). */
    if (!wait_for_transmit_complete()) {
        printf("[DEBUG] Timeout: transmit did not complete\r\n");
        return false;
    }
    if (!wait_for_receive_complete()) {
        printf("[DEBUG] Timeout: Packet not received by DMA\r\n");
        return false;
    }

    /* 6. Verify data integrity (TX buffer vs RX buffer). */
    return verify_loopback_data(Frame, rx_Frame, frame_size, "ETH_RunTestCase") ? true : false;
}