#include "eth_registers.h"
#include "reg_access.h"
#include "XMC4700.h"
#include "eth_tx_dma.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "eth_mac_cfg.h"
#include "eth_rx_dma.h"

int size = 178;

void tx_dma_stop();
void tx_dma_start();
void tx_dma_poll_demand();
void tx_set_inter_frame_gap(unsigned int data);
void tx_set_store_forward_mode();
void tx_set_cutthrough_threshold(unsigned int threshold_level);
void tx_enable_dma_interrupts();
void tx_fill_test_frame(ETH_TX_FRAME* frame);
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
void tx_desc_build_ring(ETH_TX_DESC* tx_desc_list,unsigned int* Frame_ptr,unsigned int frame_size);

// Enables store-and-forward mode for transmitting Ethernet frames.
void tx_set_store_forward_mode(){
    bits_set(ETH0_REG_OPERATION_MODE_ADDR, ETH_OPERATION_MODE_TSF_Pos, ETH_OPERATION_MODE_TSF_Msk);
}

// Configures the transmit threshold control based on the provided threshold level.
void tx_set_cutthrough_threshold(unsigned int threshold_level){
    bits_clear(ETH0_REG_OPERATION_MODE_ADDR, ETH_OPERATION_MODE_TSF_Pos, ETH_OPERATION_MODE_TSF_Msk);
    field_write(ETH0_REG_OPERATION_MODE_ADDR, ETH_OPERATION_MODE_TTC_Pos, ETH_OPERATION_MODE_TTC_Msk, threshold_level);
}

// Starts Ethernet frame transmission by setting the ST bit in the operation mode register.
void tx_dma_start(){
    bits_set(ETH0_REG_OPERATION_MODE_ADDR, ETH_OPERATION_MODE_ST_Pos, ETH_OPERATION_MODE_ST_Msk);
}

// Stops Ethernet frame transmission by clearing the ST bit in the operation mode register.
void tx_dma_stop(){
    bits_clear(ETH0_REG_OPERATION_MODE_ADDR, ETH_OPERATION_MODE_ST_Pos, ETH_OPERATION_MODE_ST_Msk);
}

// Enables Ethernet transmission and the related interrupt sources.
void tx_enable_dma_interrupts(){
    bits_set(ETH0_REG_INTERRUPT_ENABLE_ADDR, ETH_INTERRUPT_ENABLE_TIE_Pos, ETH_INTERRUPT_ENABLE_TIE_Msk);
    bits_set(ETH0_REG_INTERRUPT_ENABLE_ADDR, ETH_INTERRUPT_ENABLE_TSE_Pos, ETH_INTERRUPT_ENABLE_TSE_Msk);
    bits_set(ETH0_REG_INTERRUPT_ENABLE_ADDR, ETH_INTERRUPT_ENABLE_TUE_Pos, ETH_INTERRUPT_ENABLE_TUE_Msk);
    bits_set(ETH0_REG_INTERRUPT_ENABLE_ADDR, ETH_INTERRUPT_ENABLE_UNE_Pos, ETH_INTERRUPT_ENABLE_UNE_Msk);
    bits_set(ETH0_REG_INTERRUPT_ENABLE_ADDR, ETH_INTERRUPT_ENABLE_NIE_Pos, ETH_INTERRUPT_ENABLE_NIE_Msk);
    bits_set(ETH0_REG_INTERRUPT_ENABLE_ADDR, ETH_INTERRUPT_ENABLE_AIE_Pos, ETH_INTERRUPT_ENABLE_AIE_Msk);
    bits_set(ETH0_REG_INTERRUPT_ENABLE_ADDR, ETH_INTERRUPT_ENABLE_TJE_Pos, ETH_INTERRUPT_ENABLE_TJE_Msk);
    bits_set(ETH0_REG_MAC_CONFIGURATION_ADDR, ETH_MAC_CONFIGURATION_TE_Pos, ETH_MAC_CONFIGURATION_TE_Msk);
    bits_clear(ETH0_REG_MAC_CONFIGURATION_ADDR,ETH_MAC_CONFIGURATION_JD_Pos,ETH_MAC_CONFIGURATION_JD_Msk);
}

// Triggers DMA to poll the transmit descriptors for pending frames.
void tx_dma_poll_demand(){
    field_write(ETH0_REG_TRANSMIT_POLL_DEMAND_ADDR, ETH_TRANSMIT_POLL_DEMAND_TPD_Pos, ETH_TRANSMIT_POLL_DEMAND_TPD_Msk ,0x0);
}

// Configures the inter-frame gap in the MAC configuration register.
void tx_set_inter_frame_gap(unsigned int data){
    field_write(ETH0_REG_MAC_CONFIGURATION_ADDR,ETH_MAC_CONFIGURATION_IFG_Pos, ETH_MAC_CONFIGURATION_IFG_Msk, data);
}

// Returns the current host transmit descriptor address.
unsigned int tx_get_active_desc_addr(){
    return reg32_read(ETH0_REG_CURRENT_HOST_TRANSMIT_DESCRIPTOR_ADDR);
}

// Handles the transmission interrupt, clears status flags,
unsigned int tx_irq_flag_is_set(){
    ETH_TX_DESC* tx_desc_list = (ETH_TX_DESC*)tx_get_active_desc_addr();

	if(bit_get(ETH0_REG_STATUS_ADDR,ETH_STATUS_TI_Pos) == 1 && bit_get(ETH0_REG_STATUS_ADDR,ETH_STATUS_NIS_Pos)==1){
		bits_clear(&tx_desc_list->TDES1,31,0x80000000); //reset interrupt on completion
		bits_clear_w1c(ETH0_REG_STATUS_ADDR,ETH_STATUS_TI_Msk); //writing 1 to clear the interrupt
		/* NOTE: NIS is intentionally NOT cleared here. NIS is a shared summary
		 * bit fed by both TI and RI - on a fast internal loopback, TX and RX
		 * can both complete in the same window, and clearing NIS here was
		 * wiping out the RX side's completion signal before rx_interrupt()
		 * ever got a chance to see it (confirmed via STATUS register dumps:
		 * RI=1 and NIS=1 together on the very first check, then RI=0 for
		 * every check afterward once interrupt() had run once). */
		return 1; //to indicate transmission is successful
	}
	else if(bit_get(ETH0_REG_STATUS_ADDR,ETH_STATUS_TI_Pos) == 1 &&
		bit_get(ETH0_REG_STATUS_ADDR,ETH_STATUS_TU_Pos) == 1 &&
		field_read(ETH0_REG_STATUS_ADDR,ETH_STATUS_TS_Pos,ETH_STATUS_TS_Msk) == 0x00000006 &&
		bit_get(ETH0_REG_STATUS_ADDR,ETH_STATUS_TPS_Pos) == 1){
		bits_set(&tx_desc_list->TDES0,31,0x80000000); // set own bit to hand over the control to DMA
		bits_clear_w1c(ETH0_REG_STATUS_ADDR,ETH_STATUS_NIS_Msk);
		bits_clear_w1c(ETH0_REG_STATUS_ADDR,ETH_STATUS_TU_Msk);
		bits_clear_w1c(ETH0_REG_STATUS_ADDR,ETH_STATUS_TPS_Msk);
		return 0; //indicates DMA entered into suspended mode and need to check for transmit interrupt again in the caller
	}
	/* Neither condition was true yet - the transmission is still in flight.
	 * Previously this fell through to an unconditional "return 1", which
	 * told the caller the transmission had succeeded even though nothing
	 * had actually completed. Report "not done yet" instead so the caller's
	 * (bounded) retry loop keeps polling. */
	return 0;
}

// Allows the user to input a frame (MAC addresses, length, payload).
void tx_fill_test_frame(ETH_TX_FRAME* Frame){
	unsigned char dest_mac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; //{0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA};  //{0x18, 0xDB, 0xF2, 0x51, 0x28, 0xBC};
	memcpy(Frame -> dest_addr, dest_mac, 6);
    unsigned char sour_mac[6] =  {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA};
    memcpy(Frame -> sour_addr, sour_mac, 6);
    unsigned char len_type_mac[2] = {0x00, 0xC0};
    memcpy(Frame -> len_type, len_type_mac, 2);
//    unsigned char payload_mac[50] = {0xBB,0xCC,0xDD,0xEE, 0xAA, 0xAB, 0xAC, 0xAA, 0xAB, 0xAC, 0xAA, 0xAB, 0xAC, 0xAA, 0xAB, 0xAC, 0xAA, 0xAB, 0xAC, 0xAA, 0xAB, 0xAC, 0xAA, 0xAB, 0xAC, 0xAA, 0xAB, 0xAC, 0xAA, 0xAB, 0xAC, 0xAA, 0xAB, 0xAC, 0xAA, 0xAB, 0xAC, 0xAA, 0xAB, 0xAC, 0xAA, 0xAB, 0xAC, 0xAA, 0xAB, 0xAC, 0xAA, 0xAB, 0xAC, 0xAA, 0xFF};
//    unsigned char payload_mac[size];
    for(int i=0;i<(size/2);i++){
    	Frame -> payload[i] = 0xAB;}
    for(int i=(size/2);i<size;i++){
    	Frame -> payload[i] = 0x5D;
    }
}

void tx_fill_test_arp_frame(ETH_ARP_TX_FRAME* Frame){
		unsigned char dest_mac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
		memcpy(Frame -> dest_addr, dest_mac, 6);
	    unsigned char sour_mac[6] = {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA};
	    memcpy(Frame -> sour_addr, sour_mac, 6);
	    unsigned char len_type_mac[2] = {0x08, 0x06};
	    memcpy(Frame -> len_type, len_type_mac, 2);
//	    unsigned char payload_mac[46] = {0x00, 0x01, 0x08, 0x00, 0x06, 0x04, 0x00, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xAB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	    unsigned char hardware_type[2] = {0x00,0x01};
	    memcpy(Frame -> hw_type, hardware_type, 2);
	    unsigned char pr_type[2] = {0x08,0x00};
		memcpy(Frame -> proto_type, pr_type, 2);
		unsigned char hardware_addr_len[1] = {0x06};
		memcpy(Frame -> hw_addr_len, hardware_addr_len, 1);
		unsigned char pr_addr_len[1] = {0x04};
		memcpy(Frame -> proto_addr_len, pr_addr_len, 1);
		unsigned char op_type[2] = {0x00,0x01};
		memcpy(Frame -> op_code, op_type, 2);
		unsigned char source_mac_addr[6] = {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA};
		memcpy(Frame -> source_mac, source_mac_addr, 6);
		unsigned char source_ip_addr[4] = {0xc0,0xA8,0x02,0x1};
		memcpy(Frame -> source_ip, source_ip_addr, 4);
		unsigned char destination_mac_addr[6] = {0x0,0x0,0x0,0x0,0x0,0x0};
		memcpy(Frame -> destination_mac, destination_mac_addr, 6);
		unsigned char destination_ip_addr[4] = {0xc0,0xA8,0x03,0x9E};
		memcpy(Frame -> destination_ip, destination_ip_addr, 4);
}

// Sets the base address of the transmit descriptor list.
void tx_set_desc_list_base(ETH_TX_DESC* tx_desc_list){
    unsigned int* Tdes_ptr = (unsigned int*)tx_desc_list;
    reg32_write(ETH0_REG_TRANSMIT_DESCRIPTOR_LIST_ADDRESS_ADDR, (unsigned int)Tdes_ptr);
}

// Marks a descriptor as the first segment of a frame.
void tx_desc_mark_first_seg(ETH_TX_DESC* tx_desc_list){
    bits_set(&tx_desc_list->TDES1,29,0x20000000);
}

// Marks a descriptor as the last segment and enables interrupt on completion.
void tx_desc_mark_last_seg(unsigned int* tdes1,ETH_TX_DESC* tx_desc_list){
        bits_set(tdes1,31,0x80000000);  //interrupt on completion
        bits_set(tdes1,30,0x40000000);  //last segment 
//        unsigned int *first_descriptor = (unsigned int *)read_reg(ETH0_REG_TRANSMIT_DESCRIPTOR_LIST_ADDRESS_ADDR);
//        write_reg(&Tdes->TDES3,(unsigned int)first_descriptor);
//        unsigned int buf1_val = read_field(tdes1, 0,0x000007FF);
//                buf1_val+=4;
	if(bit_get(tdes1,24) == 0){
	    bits_set(tdes1,25,0x02000000);
	    bits_set(tdes1,22,0x00400000);
	}
}

// Configures chained descriptors for a frame.
void tx_desc_build_chained_list(ETH_TX_DESC* tx_desc_list,unsigned char* Frame_ptr,unsigned int frame_size){
   // DESCRIPTOR* next_Tdes = DESCRIPTOR++;
    //unsigned int* Tdes_ptr = (unsigned int*)next_Tdes;
	field_write(&tx_desc_list->TDES0,0, 0xF0000000, 0xF0000000);
    field_write(&tx_desc_list->TDES1,0, 0x010007FF, 0x01000040);
    reg32_write(&tx_desc_list->TDES2, (unsigned int)Frame_ptr);
    reg32_write(&tx_desc_list -> TDES3, (unsigned int)(tx_desc_list+1));
    unsigned int buffer_size = (unsigned int)(tx_desc_list->TDES1);
        buffer_size &= (0x000007FF);
    if(frame_size<buffer_size){
        buffer_size = frame_size;
	field_write(&tx_desc_list->TDES1,0, 0x000007FF, buffer_size);
    }
    frame_size -= buffer_size;
    Frame_ptr += (buffer_size);
    if(frame_size == 0){
	    tx_desc_mark_last_seg(&tx_desc_list->TDES1,tx_desc_list);

    }

	    else{
    	tx_desc_list+=1;

	tx_desc_build_chained_list(tx_desc_list, Frame_ptr, frame_size);

    }
}

// Checks for a jabber timeout error in a descriptor.
unsigned int tx_desc_has_jabber_timeout(ETH_TX_DESC* tx_desc_list){
    return bit_get(&tx_desc_list -> TDES0,15) && bit_get(&tx_desc_list -> TDES0,14) && bit_get(ETH0_REG_STATUS_ADDR, ETH_STATUS_TJT_Pos);
}

// Checks if a frame flush operation occurred.
unsigned int tx_desc_flush_pending(ETH_TX_DESC* tx_desc_list){
    return bit_get(&tx_desc_list -> TDES0,15) && bit_get(&tx_desc_list -> TDES0,13);
}

// Enables the frame flush operation in operation mode register.
void tx_enable_frame_flush(){
    bits_set(ETH0_REG_OPERATION_MODE_ADDR, ETH_OPERATION_MODE_FTF_Pos, ETH_OPERATION_MODE_FTF_Msk);
}

// Checks the status of the frame flush operation.
unsigned int tx_is_frame_flush_active(){
    return bit_get(ETH0_REG_DEBUG_ADDR, ETH_DEBUG_TRCSTS_Pos);
}

// Sets up a ring-based descriptor for frame transmission in Ethernet.
void tx_desc_build_ring(ETH_TX_DESC* tx_desc_list,unsigned int* Frame_ptr,unsigned int frame_size){
	field_write(&tx_desc_list->TDES0,0, 0xF0000000, 0xF0000000);
    field_write(&tx_desc_list->TDES1, 0, 0x000007FF, 0x000007FF);
    if(frame_size==192){
    	bits_set(&tx_desc_list->TDES0,0, 0x80000000);
    }
    reg32_write(&tx_desc_list->TDES2, (unsigned int)Frame_ptr);

    unsigned int buffer1_size = (unsigned int)(tx_desc_list->TDES1);
    unsigned int buffer2_size = 0;
    field_write(&tx_desc_list->TDES1, 11, 0x003FF800,  buffer2_size);
    buffer1_size &= (0x000007FF);
    if(frame_size <= buffer1_size){
        buffer1_size = frame_size;
	field_write(&tx_desc_list->TDES1,0,0x000007FF, buffer1_size);
    }
    else{
    field_write(&tx_desc_list->TDES1, 11,0x003FF800, 0);
    buffer2_size = (unsigned int)field_read(&tx_desc_list->TDES1,11,0x003FF800);
    buffer2_size &= (0x003FF800);
    }

    frame_size -= buffer1_size;
    Frame_ptr += (buffer1_size/4);
    if(frame_size < buffer2_size){
        buffer2_size = frame_size;
        field_write(&tx_desc_list->TDES1, 11, 0x003FF800, buffer2_size);
    }

    reg32_write(&tx_desc_list->TDES3, (unsigned int)Frame_ptr);
    frame_size -= buffer2_size;
    Frame_ptr += (buffer2_size/4);
    if(frame_size == 0){
    	tx_desc_mark_last_seg(&tx_desc_list->TDES1,tx_desc_list);

    }

    if(frame_size!=0){
    	++tx_desc_list;
//		unsigned int* next_Tdes = (unsigned int*)Tdes;
//		next_Tdes += read_descriptor_skip_length();
//		Tdes =(DESCRIPTOR*)next_Tdes;
		tx_desc_build_ring(tx_desc_list, Frame_ptr, frame_size);
    }
 }

/* Buffers/descriptors for the loopback test. Allocated once, reused for
 * every iteration of the test (re-initialised inside
 * run_internal_loopback_test() on each call).
 *
 * Pointers are handed back to the caller via the out-parameters below,
 * since the caller (main()) needs them for the lifetime of the test run
 * and to free() them at the end.
 *
 * Returns 1 on success, 0 if any allocation failed. */
unsigned int tx_rx_buff_init(ETH_TX_DESC** tx_desc_list, ETH_RX_DESC** rx_desc_list,
                              ETH_TX_FRAME** Frame, ETH_RX_FRAME** rx_Frame){

	*tx_desc_list = (ETH_TX_DESC*)malloc(sizeof(ETH_TX_DESC)*4);
	*rx_desc_list = (ETH_RX_DESC*)malloc(sizeof(ETH_RX_DESC)*4);
	*Frame        = (ETH_TX_FRAME*)malloc(sizeof(ETH_TX_FRAME));
	*rx_Frame     = (ETH_RX_FRAME*)malloc(sizeof(ETH_RX_FRAME));

	if(*tx_desc_list == NULL || *rx_desc_list == NULL || *Frame == NULL || *rx_Frame == NULL){
		printf("TEST CASE: FAIL - buffer allocation failed\n");
		return 1;
	}
	return 0;
}
