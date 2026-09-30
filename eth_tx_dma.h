#ifndef ETH_TX_DMA_H
#define ETH_TX_DMA_H

#include "eth_rx_dma.h" /* needed for ETH_RX_DESC / ETH_RX_FRAME used by tx_rx_buff_init() below */


typedef struct frame{
    //unsigned char preamble[7];
    //unsigned char SFD;
    unsigned char dest_addr[6];
    unsigned char sour_addr[6];
    unsigned char len_type[2];
    unsigned char payload[178];
//    unsigned char FCS[4];
}ETH_TX_FRAME;

typedef struct descriptor{
    unsigned int TDES0;
    unsigned int TDES1;
    unsigned int TDES2;
    unsigned int TDES3;
}ETH_TX_DESC;

typedef struct arp_frame{
    unsigned char dest_addr[6];
    unsigned char sour_addr[6];
    unsigned char len_type[2];
    unsigned char hw_type[2];
    unsigned char proto_type[2];
    unsigned char hw_addr_len[1];
    unsigned char proto_addr_len[1];
    unsigned char op_code[2];
    unsigned char source_mac[6];
    unsigned char source_ip[4];
    unsigned char destination_mac[6];
    unsigned char destination_ip[4];
//    unsigned char FCS[4];
}ETH_ARP_TX_FRAME;


void tx_dma_stop();
void tx_dma_start();
void tx_dma_poll_demand();
void tx_set_inter_frame_gap(unsigned int data);
void mac_soft_reset_start();
void mac_soft_reset_in_progress();
void mac_set_speed_10m();
void mac_set_speed_100m();
void mac_set_full_duplex();
void mac_set_half_duplex();
void mac_set_normal_frame_size();
void mac_set_jumbo_frame_size();
void tx_set_store_forward_mode();
void tx_set_cutthrough_threshold(unsigned int threshold_level);
void tx_enable_dma_interrupts();
void tx_fill_test_frame(ETH_TX_FRAME* frame);
void tx_fill_test_arp_frame(ETH_ARP_TX_FRAME* Frame);
void tx_set_desc_list_base(ETH_TX_DESC* tx_desc_list);
void tx_desc_mark_first_seg(ETH_TX_DESC* tx_desc_list);
void tx_desc_mark_last_seg(unsigned int* tdes1,ETH_TX_DESC* tx_desc_list);
void tx_desc_build_chained_list(ETH_TX_DESC* tx_desc_list,unsigned char* Frame_ptr,unsigned int frame_size);
unsigned int* frame_size();
unsigned int tx_irq_flag_is_set();
unsigned int tx_get_active_desc_addr();
unsigned int tx_desc_has_jabber_timeout(ETH_TX_DESC* tx_desc_list);
unsigned int tx_desc_flush_pending(ETH_TX_DESC* tx_desc_list);
void tx_enable_frame_flush();
unsigned int tx_is_frame_flush_active();
void mac_set_desc_skip_length();
unsigned int mac_get_desc_skip_length();
void mac_tx_use_fixed_priority();
void mac_rx_use_fixed_priority();
void mac_set_tx_weights(unsigned int priority_weights);
void mac_set_rx_weights(unsigned int priority_weights);
void tx_desc_build_ring(ETH_TX_DESC* tx_desc_list,unsigned int* Frame_ptr,unsigned int frame_size);
unsigned int tx_rx_buff_init(ETH_TX_DESC** tx_desc_list, ETH_RX_DESC** rx_desc_list,ETH_TX_FRAME** Frame, ETH_RX_FRAME** rx_Frame);
#endif /* ETH_TX_DMA_H */
