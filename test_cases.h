#ifndef TEST_CASES_H_
#define TEST_CASES_H_

#include <stdint.h>
#include <stdbool.h>
#include "eth_tx_dma.h"
#include "eth_rx_dma.h"

typedef enum {
    ETH_SPEED_10M  = 0,
    ETH_SPEED_100M = 1
} eth_speed_t;

typedef enum {
    ETH_DUPLEX_HALF = 0,
    ETH_DUPLEX_FULL = 1
} eth_duplex_t;

typedef enum {
    ETH_TEST_MAC_10M_HALF  = 1,
    ETH_TEST_MAC_10M_FULL  = 2,
    ETH_TEST_MAC_100M_HALF = 3,
    ETH_TEST_MAC_100M_FULL = 4,
    ETH_TEST_PHY_10M_HALF  = 5,
    ETH_TEST_PHY_10M_FULL  = 6,
    ETH_TEST_PHY_100M_HALF = 7,
    ETH_TEST_PHY_100M_FULL = 8
} eth_test_id_t;

#ifndef LOOPBACK_TEST_RUNS
#define LOOPBACK_TEST_RUNS   5u   /* number of times each loopback test is repeated */
#endif

/* Helpers implemented in main.c, shared with test_cases.c */
int wait_for_transmit_complete(void);
int wait_for_receive_complete(void);
int verify_loopback_data(ETH_TX_FRAME *Frame, ETH_RX_FRAME *rx_Frame, unsigned int frame_size, const char* test_label);

/* Single unified declaration */
bool ETH_RunTestCase(eth_test_id_t test_id,
                     ETH_TX_FRAME* Frame,
                     ETH_RX_FRAME* rx_Frame,
                     ETH_TX_DESC* tx_desc_list,
                     ETH_RX_DESC* rx_desc_list);

#endif /* TEST_CASES_H_ */
