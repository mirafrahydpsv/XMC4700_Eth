#ifndef ETH_MAC_CFG_H_
#define ETH_MAC_CFG_H_
#include <stdint.h>
#include <stdbool.h>
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
void eth_select_rmii_interface(void);

/* Automation MAC Configuration */
void ETH_MAC_ConfigureMode(bool mac_loopback, uint8_t speed_100m, uint8_t full_duplex);

#endif
