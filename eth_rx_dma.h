#ifndef ETH_RX_DMA_H
#define ETH_RX_DMA_H

typedef struct rx_frame{
    //unsigned char preamble[7];
    //unsigned char SFD;
    unsigned char dest_addr[6];
    unsigned char sour_addr[6];
    unsigned char len_type[2];
    unsigned char payload[178];
//    unsigned char FCS[4];
}ETH_RX_FRAME;

typedef struct rx_descriptor{
    unsigned int RDES0;
    unsigned int RDES1;
    unsigned int RDES2;
    unsigned int RDES3;
}ETH_RX_DESC;

typedef struct rx_arp_frame{
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
}ETH_ARP_RX_FRAME;

void rx_dma_stop();
void rx_dma_start();
void rx_dma_poll_demand();
void rx_set_store_forward_mode();
void rx_set_cutthrough_threshold(unsigned int threshold_level);
void rx_enable_dma_interrupts();
void rx_set_desc_list_base(ETH_RX_DESC* rx_desc_list);
void rx_desc_mark_first_seg(ETH_RX_DESC* rx_desc_list);
void rx_desc_mark_last_seg(unsigned int* rdes0, unsigned int* rdes1);
void rx_desc_build_chained_list(ETH_RX_DESC* rx_desc_list,unsigned char* Frame_ptr,unsigned int frame_size);
unsigned int rx_irq_flag_is_set();
unsigned int rx_get_active_desc_addr();
void rx_disable_frame_flush();
unsigned int rx_is_frame_flush_active();
void rx_desc_build_ring(ETH_RX_DESC* rx_desc_list,unsigned int* Frame_ptr,unsigned int frame_size);
unsigned int rx_desc_has_watchdog_timeout(ETH_RX_DESC* rx_desc_list);
unsigned int rx_desc_has_crc_error(ETH_RX_DESC* rx_desc_list);
unsigned int rx_desc_has_error(ETH_RX_DESC* rx_desc_list);

#endif /* ETH_RX_DMA_H */
