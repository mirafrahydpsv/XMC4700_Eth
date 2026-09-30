#include "eth_registers.h"
#include "reg_access.h"
#include "XMC4700.h"
#include "eth_tx_dma.h"
#include <stdio.h>
#include "eth_mac_cfg.h"
#include "eth_rx_dma.h"

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

// Enables store-and-forward mode for transmitting Ethernet frames.
void rx_set_store_forward_mode(){
    bits_set(ETH0_REG_OPERATION_MODE_ADDR, ETH_OPERATION_MODE_RSF_Pos, ETH_OPERATION_MODE_RSF_Msk);
}

// Configures the transmit threshold control based on the provided threshold level.
void rx_set_cutthrough_threshold(unsigned int threshold_level){
    bits_clear(ETH0_REG_OPERATION_MODE_ADDR, ETH_OPERATION_MODE_RSF_Pos, ETH_OPERATION_MODE_RSF_Msk);
    field_write(ETH0_REG_OPERATION_MODE_ADDR, ETH_OPERATION_MODE_RTC_Pos, ETH_OPERATION_MODE_RTC_Msk, threshold_level);
}

// Starts Ethernet frame transmission by setting the ST bit in the operation mode register.
void rx_dma_start(){
    bits_set(ETH0_REG_OPERATION_MODE_ADDR, ETH_OPERATION_MODE_SR_Pos, ETH_OPERATION_MODE_SR_Msk);
}

// Stops Ethernet frame transmission by clearing the ST bit in the operation mode register.
void rx_dma_stop(){
    bits_clear(ETH0_REG_OPERATION_MODE_ADDR, ETH_OPERATION_MODE_SR_Pos, ETH_OPERATION_MODE_SR_Msk);
}

// Enables Ethernet transmission and the related interrupt sources.
void rx_enable_dma_interrupts(){
    bits_set(ETH0_REG_INTERRUPT_ENABLE_ADDR, ETH_INTERRUPT_ENABLE_RIE_Pos, ETH_INTERRUPT_ENABLE_RIE_Msk);
    bits_set(ETH0_REG_INTERRUPT_ENABLE_ADDR, ETH_INTERRUPT_ENABLE_RSE_Pos, ETH_INTERRUPT_ENABLE_RSE_Msk);
    bits_set(ETH0_REG_INTERRUPT_ENABLE_ADDR, ETH_INTERRUPT_ENABLE_RUE_Pos, ETH_INTERRUPT_ENABLE_RUE_Msk);
    bits_set(ETH0_REG_INTERRUPT_ENABLE_ADDR, ETH_INTERRUPT_ENABLE_UNE_Pos, ETH_INTERRUPT_ENABLE_UNE_Msk);
    bits_set(ETH0_REG_INTERRUPT_ENABLE_ADDR, ETH_INTERRUPT_ENABLE_NIE_Pos, ETH_INTERRUPT_ENABLE_NIE_Msk);
    bits_set(ETH0_REG_INTERRUPT_ENABLE_ADDR, ETH_INTERRUPT_ENABLE_AIE_Pos, ETH_INTERRUPT_ENABLE_AIE_Msk);
    bits_set(ETH0_REG_INTERRUPT_ENABLE_ADDR, ETH_INTERRUPT_ENABLE_RWE_Pos, ETH_INTERRUPT_ENABLE_RWE_Msk);
    bits_set(ETH0_REG_MAC_CONFIGURATION_ADDR, ETH_MAC_CONFIGURATION_RE_Pos, ETH_MAC_CONFIGURATION_RE_Msk);
    bits_clear(ETH0_REG_MAC_CONFIGURATION_ADDR,ETH_MAC_CONFIGURATION_WD_Pos,ETH_MAC_CONFIGURATION_WD_Msk);
}

// Triggers DMA to poll the transmit descriptors for pending frames.
void rx_dma_poll_demand(){
    reg32_write(ETH0_REG_RECEIVE_POLL_DEMAND_ADDR, 0xAAABCDEF);
}

// Returns the current host transmit descriptor address.
unsigned int rx_get_active_desc_addr(){
    return reg32_read(ETH0_REG_CURRENT_HOST_RECEIVE_DESCRIPTOR_ADDR);
}

// Handles the transmission interrupt, clears status flags,
unsigned int rx_irq_flag_is_set(){
	ETH_RX_DESC* rx_desc_list = (ETH_RX_DESC*)rx_get_active_desc_addr();

	if(bit_get(ETH0_REG_STATUS_ADDR,ETH_STATUS_RI_Pos) == 1 && bit_get(ETH0_REG_STATUS_ADDR,ETH_STATUS_NIS_Pos)==1){
		bits_clear(&rx_desc_list->RDES1,31,0x80000000); //reset interrupt on completion
		bits_clear_w1c(ETH0_REG_STATUS_ADDR,ETH_STATUS_RI_Msk); //writing 1 to clear the interrupt
		/* NOTE: NIS is intentionally NOT cleared here - see interrupt() in
		 * ETH_data_transmission_dma_mode.c for why. Same shared-bit race,
		 * mirrored for the RX side. */
		return 1; //to indicate reception is successful
	}
	else if(bit_get(ETH0_REG_STATUS_ADDR,ETH_STATUS_RI_Pos) == 1 &&
		bit_get(ETH0_REG_STATUS_ADDR,ETH_STATUS_RU_Pos) == 1 &&
		field_read(ETH0_REG_STATUS_ADDR,ETH_STATUS_RS_Pos,ETH_STATUS_RS_Msk) == 0x00000006 &&
		bit_get(ETH0_REG_STATUS_ADDR,ETH_STATUS_RPS_Pos) == 1){
		bits_set(&rx_desc_list->RDES0,31,0x80000000); // set own bit to hand over the control to DMA
		bits_clear_w1c(ETH0_REG_STATUS_ADDR,ETH_STATUS_NIS_Msk);
		bits_clear_w1c(ETH0_REG_STATUS_ADDR,ETH_STATUS_RU_Msk);
		bits_clear_w1c(ETH0_REG_STATUS_ADDR,ETH_STATUS_RPS_Msk);
		return 0; //indicates DMA entered into suspended mode and need to check for receive interrupt again in the caller
	}
	/* Not done yet - previously fell through to an unconditional "return 1"
	 * (false positive). Report "not ready" so the caller's bounded retry
	 * loop keeps polling instead of trusting a receive that hasn't happened. */
	return 0;
}

// Sets the base address of the transmit descriptor list.
void rx_set_desc_list_base(ETH_RX_DESC* rx_desc_list){
    unsigned int* Rdes_ptr = (unsigned int*)rx_desc_list;
    reg32_write(ETH0_REG_RECEIVE_DESCRIPTOR_LIST_ADDRESS_ADDR, (unsigned int)Rdes_ptr);
}

// Marks a descriptor as the first segment of a frame.
void rx_desc_mark_first_seg(ETH_RX_DESC* rx_desc_list){
    bits_set(&rx_desc_list->RDES0,9,0x00000200);
}

// Marks a descriptor as the last segment and enables interrupt on completion.
void rx_desc_mark_last_seg(unsigned int* rdes0,unsigned int* rdes1){
	bits_clear(rdes1,31,0x80000000);  //clear disable interrupt on completion
        unsigned int buf1_val = field_read(rdes1, 0,0x000007FF);
        buf1_val+=4;
        field_write(rdes1,0,0x000007FF,buf1_val);
	if(bit_get(rdes1,24) == 0){
	    bits_set(rdes1,25,0x02000000);
	}
}

// Configures chained descriptors for a frame.
void rx_desc_build_chained_list(ETH_RX_DESC* rx_desc_list,unsigned char* Frame_ptr,unsigned int frame_size){
   // DESCRIPTOR* next_Tdes = DESCRIPTOR++;
    //unsigned int* Tdes_ptr = (unsigned int*)next_Tdes;
	field_write(&rx_desc_list->RDES0,0, 0x80000000, 0x80000000);
    field_write(&rx_desc_list->RDES1,0,0x810007FF, 0x810007FF);
    reg32_write(&rx_desc_list->RDES2, (unsigned int)Frame_ptr);
	reg32_write(&rx_desc_list -> RDES3, (unsigned int)(rx_desc_list+1));

    unsigned int buffer_size = (unsigned int)(rx_desc_list->RDES1);
    buffer_size &= (0x000007FF);
    if(frame_size<buffer_size){
        buffer_size = frame_size;
	    field_write(&rx_desc_list->RDES1,0,0x000007FF,buffer_size);
    }
    frame_size -= buffer_size;
    Frame_ptr += (buffer_size);
    if(frame_size==0){
    	rx_desc_mark_last_seg(&rx_desc_list->RDES0,&rx_desc_list->RDES1);
    }
    if(frame_size!=0){
    	rx_desc_list+=1;

	rx_desc_build_chained_list(rx_desc_list, Frame_ptr, frame_size);
    }
}

void rx_disable_frame_flush(){
    bits_set(ETH0_REG_OPERATION_MODE_ADDR, ETH_OPERATION_MODE_DFF_Pos, ETH_OPERATION_MODE_DFF_Msk);
}

// Checks the status of the frame flush operation.
unsigned int rx_is_frame_flush_active(){
    return bit_get(ETH0_REG_DEBUG_ADDR, ETH_DEBUG_RRCSTS_Pos);
}

unsigned int rx_desc_has_watchdog_timeout(ETH_RX_DESC* rx_desc_list){
    return bit_get(&rx_desc_list -> RDES0,15) && bit_get(&rx_desc_list -> RDES0,4) && bit_get(ETH0_REG_STATUS_ADDR, ETH_STATUS_RWT_Pos);

}

unsigned int rx_desc_has_crc_error(ETH_RX_DESC* rx_desc_list){
    return bit_get(&rx_desc_list -> RDES0,15) && bit_get(&rx_desc_list -> RDES0,1);

}

unsigned int rx_desc_has_error(ETH_RX_DESC* rx_desc_list){
    return bit_get(&rx_desc_list -> RDES0,15) && bit_get(&rx_desc_list -> RDES0,14);

}

// Sets up a ring-based descriptor for frame transmission in Ethernet.
void rx_desc_build_ring(ETH_RX_DESC* rx_desc_list,unsigned int* Frame_ptr,unsigned int frame_size){
	rx_desc_mark_first_seg(rx_desc_list);

	field_write(&rx_desc_list->RDES0,0, 0x80000000, 0x80000000);
    field_write(&rx_desc_list->RDES1, 0,0x800007FF, 0x800007FF);
    reg32_write(&rx_desc_list->RDES2, (unsigned int)Frame_ptr);

    unsigned int buffer1_size = (unsigned int)(rx_desc_list->RDES1);
    unsigned int buffer2_size = 0;
    field_write(&rx_desc_list->RDES1, 11,  0x003FF800, buffer2_size);
    buffer1_size &= (0x000007FF);
    if(frame_size <= buffer1_size){
        buffer1_size = frame_size;
        field_write(&rx_desc_list->RDES1,0,0x000007FF, buffer1_size);
    }
    else{
    field_write(&rx_desc_list->RDES1, 11, 0x003FF800, 0);
    buffer2_size = (unsigned int)field_read(&rx_desc_list->RDES1,11,0x003FF800);
    buffer2_size &= (0x003FF800);
    }

    frame_size -= buffer1_size;
    Frame_ptr += (buffer1_size/4);
    if(frame_size < buffer2_size){
        buffer2_size = frame_size;
        field_write(&rx_desc_list->RDES1, 11, 0x003FF800,buffer2_size);
    }

    reg32_write(&rx_desc_list->RDES3, (unsigned int)Frame_ptr);
    frame_size -= buffer2_size;
    Frame_ptr += (buffer2_size/4);

    if(frame_size == 0){
    	  rx_desc_mark_last_seg(&rx_desc_list->RDES0,&rx_desc_list->RDES1);
     }

    if(frame_size!=0){
	++rx_desc_list;
//	unsigned int* next_Rdes = (unsigned int*)Rdes;
//	next_Rdes += read_descriptor_skip_length();
//	Rdes =(RX_DESCRIPTOR*)next_Rdes;
	rx_desc_build_ring(rx_desc_list, Frame_ptr, frame_size);
    }
}

