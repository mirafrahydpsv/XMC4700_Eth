#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "reg_access.h"
#include "eth_registers.h"
#include "XMC4700.h"
#include "eth_tx_dma.h"
#include "eth_rx_dma.h"
#include "eth_phy_mdio.h"
#include "eth_power_clock.h"
#include "eth_mac_cfg.h"
#include "test_cases.h"
//#include "DAVE.h"
#include "debug_uart.h"
#include <string.h>
#include <stdlib.h>

int uart_console_init(void); /* defined in uart.c - was missing a prototype, causing an implicit-declaration warning */

int count = 0;

void dump_test_buffers(ETH_TX_DESC *tx_desc_list,ETH_RX_DESC *rx_desc_list,ETH_TX_FRAME* Frame,ETH_RX_FRAME* rx_Frame);
int n=192;

#define LOOPBACK_MAX_RETRIES   20u   /* bounded retries so a failed test reports FAIL instead of hanging forever */
//#define LOOPBACK_TEST_RUNS      5u   /* number of times the internal loopback test case is repeated in main() */

void clock_init_pll(unsigned int n,unsigned int p,unsigned int k2){
	// n = 47, p = 1, k2 = 0 for 144 MHZ
	bits_clear(SCU_PLL_PLLCON0_ADDR,SCU_PLL_PLLCON0_VCOPWD_Pos,SCU_PLL_PLLCON0_VCOPWD_Msk);  	//Configure VCO to active mode by clearing
	bits_set(SCU_REG_SYSCLKCR_ADDR,SCU_CLK_SYSCLKCR_SYSSEL_Pos,SCU_CLK_SYSCLKCR_SYSSEL_Msk);	//System Clock Selection Value - set to fPLL
	bits_clear(SCU_PLL_PLLCON2_ADDR,SCU_PLL_PLLCON2_PINSEL_Pos,SCU_PLL_PLLCON2_PINSEL_Msk);		//P-Divider Input Selection - 0 - PLL external oscillator selected
	bits_clear(SCU_PLL_PLLCON0_ADDR,SCU_PLL_PLLCON0_PLLPWD_Pos,SCU_PLL_PLLCON0_PLLPWD_Msk);		//Configure PLL to active mode by clearing
	field_write(SCU_REG_EXTCLKCR_ADDR,SCU_CLK_EXTCLKCR_ECKSEL_Pos, SCU_CLK_EXTCLKCR_ECKSEL_Msk, 0x3); 						//fPLL clock divided according to ECKDIV bit field configuration
	field_write(SCU_REG_EXTCLKCR_ADDR,SCU_CLK_EXTCLKCR_ECKDIV_Pos, SCU_CLK_EXTCLKCR_ECKDIV_Msk, 0x0);							//External Clock Divider Value
	field_write(SCU_OSC_OSCHPCTRL_ADDR,SCU_OSC_OSCHPCTRL_GAINSEL_Pos, SCU_OSC_OSCHPCTRL_GAINSEL_Msk, 0x01);						// Oscillator Gain Selection -  01 - The gain control is configured for frequencies from 4MHz to 16MHz
	field_write(SCU_PLL_PLLCON1_ADDR,SCU_PLL_PLLCON1_NDIV_Pos,SCU_PLL_PLLCON1_NDIV_Msk, n);								//N-Divider Value
	field_write(SCU_PLL_PLLCON1_ADDR,SCU_PLL_PLLCON1_PDIV_Pos,SCU_PLL_PLLCON1_PDIV_Msk,p);								//P-Divider Value
	field_write(SCU_PLL_PLLCON1_ADDR,SCU_PLL_PLLCON1_K2DIV_Pos,SCU_PLL_PLLCON1_K2DIV_Msk,k2);								//K2-Divider Value
}

/*void config_ports(){
	//TXD0 - p2.8
	set_bit(PORT2_OMR_ADDR,PORT2_OMR_PS8_Pos,PORT2_OMR_PS8_Msk);
	clear_bit(PORT2_OMR_ADDR,PORT2_OMR_PR8_Pos,PORT2_OMR_PR8_Msk);
	write_field(PORT2_HWSEL_ADDR,PORT2_HWSEL_HW8_Pos, PORT2_HWSEL_HW8_Msk, 0x1);
	write_field(PORT2_PDR1_ADDR, PORT2_PDR1_PD8_Pos, PORT2_PDR1_PD8_Msk, 0x2);  		 	 //pad type - A2, functionality - strong driver, soft edge - 0x2
	write_field(PORT2_IOCR8_ADDR, PORT2_IOCR8_PC8_Pos, PORT2_IOCR8_PC8_Msk, 0x19);		 	 //ALT1 - 19

	//TXD1 - p2.9
	set_bit(PORT2_OMR_ADDR,PORT2_OMR_PS9_Pos,PORT0_OMR_PS9_Msk);
	clear_bit(PORT2_OMR_ADDR,PORT2_OMR_PR9_Pos,PORT2_OMR_PR9_Msk);
	write_field(PORT2_HWSEL_ADDR,PORT2_HWSEL_HW9_Pos, PORT2_HWSEL_HW9_Msk, 0x1);
	write_field(PORT2_PDR1_ADDR,PORT2_PDR1_PD9_Pos, PORT2_PDR1_PD9_Msk, 0x2);  			 //pad type - A2, functionality - strong driver, soft edge - 0x2
	write_field(PORT2_IOCR8_ADDR,PORT2_IOCR8_PC9_Pos, PORT2_IOCR8_PC9_Msk, 0x19);		 	 //ALT1 - 19

	//ETH0 MDC - p2.7
	set_bit(PORT2_OMR_ADDR,PORT2_OMR_PS7_Pos,PORT2_OMR_PS7_Msk);
	clear_bit(PORT2_OMR_ADDR,PORT2_OMR_PR7_Pos,PORT2_OMR_PR7_Msk);
	write_field(PORT2_HWSEL_ADDR,PORT2_HWSEL_HW7_Pos, PORT2_HWSEL_HW7_Msk, 0x1);
	write_field(PORT2_PDR0_ADDR,PORT2_PDR0_PD7_Pos, PORT2_PDR0_PD7_Msk, 0x2);  		     //pad type - A1+, functionality -  strong driver, soft edge - 0x2
	write_field(PORT2_IOCR4_ADDR,PORT2_IOCR4_PC7_Pos, PORT2_IOCR4_PC7_Msk, 0x19);		 	 //ALT1 - 19

	//ETH0  ETH0.CLK_RMIIC - p15.8
	write_field(PORT15_IOCR8_ADDR,PORT15_IOCR8_PC8_Pos, PORT15_IOCR8_PC8_Msk, 0x0);   //No internal pull device active
	clear_bit(PORT15_PDISC_ADDR , PORT15_PDISC_PDIS8_Pos , PORT15_PDISC_PDIS8_Msk);

	//ETH0  ETH0.CRS_DVC - p15.9
	write_field(PORT15_IOCR8_ADDR,PORT15_IOCR8_PC9_Pos, PORT15_IOCR8_PC9_Msk, 0x00);			//No internal pull device active
	clear_bit(PORT15_PDISC_ADDR , PORT15_PDISC_PDIS9_Pos , PORT15_PDISC_PDIS9_Msk);

	//TXEN - p2.5
	set_bit(PORT2_OMR_ADDR,PORT2_OMR_PS5_Pos,PORT2_OMR_PS5_Msk);
	clear_bit(PORT2_OMR_ADDR,PORT2_OMR_PR5_Pos,PORT2_OMR_PR5_Msk);
	write_field(PORT2_HWSEL_ADDR,PORT2_HWSEL_HW5_Pos, PORT2_HWSEL_HW5_Msk, 0x1);
	write_field(PORT2_PDR0_ADDR,PORT2_PDR0_PD5_Pos, PORT2_PDR0_PD5_Msk, 0x2);  		 	 //pad type - A2, functionality - strong driver, soft edge - 0x2
	write_field(PORT2_IOCR4_ADDR,PORT2_IOCR4_PC5_Pos, PORT2_IOCR4_PC5_Msk, 0x19);		 	 //ALT1 - 19

	//RX_ERA - p2.4
	write_field(PORT2_PDR0_ADDR,PORT2_PDR0_PD4_Pos, PORT2_PDR0_PD4_Msk, 0x2);  		 	 //pad type - A2, functionality - strong driver, soft edge - 0x2
	write_field(PORT2_IOCR4_ADDR,PORT2_IOCR4_PC4_Pos, PORT2_IOCR4_PC4_Msk, 0x00);			//No internal pull device active

	//RXD0A - p2.2
	write_field(PORT2_PDR0_ADDR,PORT2_PDR0_PD2_Pos, PORT2_PDR0_PD2_Msk, 0x2);  		 	 //pad type - A2, functionality - strong driver, soft edge - 0x2
	write_field(PORT2_IOCR0_ADDR,PORT2_IOCR0_PC2_Pos, PORT2_IOCR0_PC2_Msk, 0x00);			//No internal pull device active

	//RXD1A - p2.3
	write_field(PORT2_PDR0_ADDR,PORT2_PDR0_PD3_Pos, PORT2_PDR0_PD3_Msk, 0x2);  		 	 //pad type - A2, functionality - strong driver, soft edge - 0x2
	write_field(PORT2_IOCR0_ADDR,PORT2_IOCR0_PC3_Pos,PORT2_IOCR0_PC3_Msk, 0x00);			//No internal pull device active

	//Management Data In/Out - ETH0.MDIB / ETH0.MDO - p2.0
	write_field(PORT2_PDR0_ADDR,PORT2_PDR0_PD0_Pos,PORT2_PDR0_PD0_Msk, 0x2);  		 	 //pad type - A2, functionality - strong driver, soft edge - 0x2
	write_field(PORT2_HWSEL_ADDR,PORT2_HWSEL_HW0_Pos, PORT2_HWSEL_HW0_Msk, 0x1);			//HWI0/HWO0 control path can override the software configuration.

	//EXTSYSCLK - p0.8
	set_bit(PORT0_OMR_ADDR,PORT0_OMR_PS8_Pos,PORT0_OMR_PS8_Msk);
	clear_bit(PORT0_OMR_ADDR,PORT0_OMR_PR8_Pos,PORT0_OMR_PR8_Msk);
	write_field(PORT0_PDR1_ADDR,PORT0_PDR1_PD8_Pos, PORT0_PDR1_PD8_Msk, 0x2);  		 	 //pad type - A2, functionality - strong driver, soft edge - 0x2
	write_field(PORT0_IOCR8_ADDR,PORT0_IOCR8_PC8_Pos, PORT0_IOCR8_PC8_Msk, 0x11);		 	 //ALT1 - 19
	write_field(PORT0_HWSEL_ADDR,PORT0_HWSEL_HW8_Pos, PORT0_HWSEL_HW8_Msk, 0x2);

//	//INTERRUPT - 1.3
	write_field(PORT1_PDR0_ADDR,PORT2_PDR0_PD3_Pos, PORT1_PDR0_PD3_Msk, 0x2);  		 	 //pad type - A2, functionality - strong driver, soft edge - 0x2
	write_field(PORT1_IOCR0_ADDR,PORT1_IOCR0_PC3_Pos, PORT1_IOCR0_PC3_Msk, 0x00);

}*/

void gpio_init_eth_pins(){
	//Configure ETH0.MDIO (P2.0), ETH0.RXD0A (P2.2) and ETH0.RXD1A (P2.3)

	field_write(PORT2_IOCR0_ADDR,PORT2_IOCR0_PC0_Pos, PORT2_IOCR0_PC0_Msk, 0x00);
	field_write(PORT2_IOCR0_ADDR,PORT2_IOCR0_PC2_Pos, PORT2_IOCR0_PC2_Msk, 0x00);
	field_write(PORT2_IOCR0_ADDR,PORT2_IOCR0_PC3_Pos,PORT2_IOCR0_PC3_Msk, 0x00);

	field_write(PORT2_IOCR4_ADDR,PORT2_IOCR4_PC4_Pos, PORT2_IOCR4_PC4_Msk, 0x00);
	field_write(PORT2_IOCR4_ADDR,PORT2_IOCR4_PC5_Pos, PORT2_IOCR4_PC5_Msk, 0x11);
	field_write(PORT2_IOCR4_ADDR,PORT2_IOCR4_PC7_Pos, PORT2_IOCR4_PC7_Msk, 0x11);

	field_write(PORT2_IOCR8_ADDR, PORT2_IOCR8_PC8_Pos, PORT2_IOCR8_PC8_Msk, 0x11);
	field_write(PORT2_IOCR8_ADDR,PORT2_IOCR8_PC9_Pos, PORT2_IOCR8_PC9_Msk, 0x11);
	field_write(PORT15_IOCR8_ADDR,PORT15_IOCR8_PC8_Pos, PORT15_IOCR8_PC8_Msk, 0x0);
	field_write(PORT15_IOCR8_ADDR,PORT15_IOCR8_PC9_Pos, PORT15_IOCR8_PC9_Msk, 0x00);

	field_write(PORT2_HWSEL_ADDR,PORT2_HWSEL_HW0_Pos, PORT2_HWSEL_HW0_Msk, 0x1);
	field_write(PORT2_PDR0_ADDR,PORT2_PDR0_PD5_Pos, PORT2_PDR0_PD5_Msk, 0x0);
	field_write(PORT2_PDR1_ADDR, PORT2_PDR1_PD8_Pos, PORT2_PDR1_PD8_Msk, 0x0);
	field_write(PORT2_PDR1_ADDR,PORT2_PDR1_PD9_Pos, PORT2_PDR1_PD9_Msk, 0x0);
	bits_clear(PORT15_PDISC_ADDR , PORT15_PDISC_PDIS8_Pos , PORT15_PDISC_PDIS8_Msk);
	bits_clear(PORT15_PDISC_ADDR , PORT15_PDISC_PDIS9_Pos , PORT15_PDISC_PDIS9_Msk);

}

void run_rx_frame_capture(ETH_RX_FRAME* rx_Frame, ETH_RX_DESC* rx_desc_list,unsigned char mode,unsigned char rdes_type,unsigned int frame_size,unsigned int threshold_level){
	unsigned char* rx_Frame_ptr = (unsigned char*)rx_Frame;
	if(mode == 's'){
		rx_set_store_forward_mode();    //s defines the store and forward mode
	}
	else if(mode == 't'){
		rx_set_cutthrough_threshold(threshold_level);
	}
	rx_dma_stop();
	rx_set_desc_list_base(rx_desc_list);
	rx_desc_mark_first_seg(rx_desc_list);
	if(rdes_type == 'c'){
		rx_desc_build_chained_list(rx_desc_list,rx_Frame_ptr,frame_size);
	}
	else if(rdes_type == 'r'){
		rx_desc_build_ring(rx_desc_list,(unsigned int*)rx_Frame_ptr,frame_size);
	}
	while(rx_irq_flag_is_set() == 0){
		rx_dma_poll_demand();
		rx_dma_start();
	}
}

unsigned int rx_get_status_flags(unsigned char error_summary){
    ETH_RX_DESC* rx_desc_list = (ETH_RX_DESC*)rx_get_active_desc_addr();
    if(error_summary == 'w'){
        return rx_desc_has_watchdog_timeout(rx_desc_list);
    }
    else if(error_summary == 'c'){
        return rx_desc_has_crc_error(rx_desc_list);
    }
    else if(error_summary == 'd'){
    	return rx_desc_has_error(rx_desc_list);
    }
    return 0;
}

void run_tx_frame_send(ETH_TX_FRAME* Frame, ETH_TX_DESC* tx_desc_list, unsigned char mode, unsigned char tdes_type, unsigned int frame_size,unsigned int threshold_level,unsigned int frame_gap,ETH_RX_FRAME* rx_Frame, ETH_RX_DESC* rx_desc_list){
//    unsigned int* Frame_ptr = (unsigned int*)Frame;
    if(mode == 's'){
    	tx_set_store_forward_mode(); //s defines the store and forward mode
    }
    else if(mode == 't'){
    	tx_set_cutthrough_threshold(threshold_level);
    }
    tx_dma_stop();
    tx_set_desc_list_base(tx_desc_list);
    tx_fill_test_frame(Frame);
//    unsigned int *tdes_ptr = (unsigned int*)Tdes;
//    tdes_ptr+=35;
//    Tdes = (DESCRIPTOR*)tdes_ptr;
    unsigned char* Frame_ptr = (unsigned char*)Frame;
//    Frame_ptr = Frame_ptr+14;
    tx_desc_mark_first_seg(tx_desc_list);
    if(tdes_type == 'c'){
    	tx_desc_build_chained_list(tx_desc_list,Frame_ptr,frame_size);
    }
    else if(tdes_type == 'r'){
    	tx_desc_build_ring(tx_desc_list, (unsigned int*)Frame_ptr,frame_size);
    }

    tx_dma_poll_demand();
    mac_apply_config('l', 'f', 'n');
    printf("Values before starting the transmission\n");
    dump_test_buffers(tx_desc_list,rx_desc_list,Frame,rx_Frame);

//    for(int i=0;i<10000;i++){
//        	for(int j=0;j<10;j++){
//        	}
//        }
    printf("\n\n");

    while(tx_irq_flag_is_set() == 0){
        tx_dma_poll_demand();
	    tx_dma_start();
    }
    tx_set_inter_frame_gap(frame_gap);
}

void run_rx_arp_capture(ETH_ARP_RX_FRAME* rx_Frame, ETH_RX_DESC* rx_desc_list,unsigned char mode,unsigned char rdes_type,unsigned int frame_size,unsigned int threshold_level){
	unsigned char* rx_Frame_ptr = (unsigned char*)rx_Frame;
	if(mode == 's'){
		rx_set_store_forward_mode();    //s defines the store and forward mode
	}
	else if(mode == 't'){
		rx_set_cutthrough_threshold(threshold_level);
	}
	rx_dma_stop();
	rx_set_desc_list_base(rx_desc_list);
	rx_desc_mark_first_seg(rx_desc_list);
	if(rdes_type == 'c'){
		rx_desc_build_chained_list(rx_desc_list,rx_Frame_ptr,frame_size);
	}
	else if(rdes_type == 'r'){
		rx_desc_build_ring(rx_desc_list,(unsigned int*)rx_Frame_ptr,frame_size);
	}
	while(rx_irq_flag_is_set() == 0){
		rx_dma_poll_demand();
		rx_dma_start();
	}
}

void run_tx_arp_send(ETH_ARP_TX_FRAME* Frame, ETH_TX_DESC* tx_desc_list, unsigned char mode, unsigned char tdes_type, unsigned int frame_size,unsigned int threshold_level,unsigned int frame_gap,ETH_ARP_RX_FRAME* rx_Frame, ETH_RX_DESC* rx_desc_list){
//    unsigned int* Frame_ptr = (unsigned int*)Frame;
    if(mode == 's'){
    	tx_set_store_forward_mode(); //s defines the store and forward mode
    }
    else if(mode == 't'){
    	tx_set_cutthrough_threshold(threshold_level);
    }
    tx_dma_stop();
    tx_set_desc_list_base(tx_desc_list);
    tx_fill_test_arp_frame(Frame);
//    unsigned int *tdes_ptr = (unsigned int*)Tdes;
//    tdes_ptr+=35;
//    Tdes = (DESCRIPTOR*)tdes_ptr;
    unsigned char* Frame_ptr = (unsigned char*)Frame;
//    Frame_ptr = Frame_ptr+14;
    tx_desc_mark_first_seg(tx_desc_list);
    if(tdes_type == 'c'){
    	tx_desc_build_chained_list(tx_desc_list,Frame_ptr,frame_size);
    }
    else if(tdes_type == 'r'){
    	tx_desc_build_ring(tx_desc_list, (unsigned int*)Frame_ptr,frame_size);
    }

    tx_dma_poll_demand();
    mac_apply_config('l', 'f', 'n');
    printf("Values before starting the transmission\n");
    dump_test_buffers(tx_desc_list,rx_desc_list,(ETH_TX_FRAME*)Frame,(ETH_RX_FRAME*)rx_Frame);

//    for(int i=0;i<10000;i++){
//        	for(int j=0;j<10;j++){
//        	}
//        }
    printf("\n\n");

    while(tx_irq_flag_is_set() == 0){
        tx_dma_poll_demand();
	    tx_dma_start();
    }
    tx_set_inter_frame_gap(frame_gap);
}

unsigned int tx_get_status_flags(unsigned char error_summary){
    ETH_TX_DESC* tx_desc_list = (ETH_TX_DESC*)tx_get_active_desc_addr();
    if(error_summary == 'j'){
	return tx_desc_has_jabber_timeout(tx_desc_list);
    } else if(error_summary == 'f'){
	return tx_desc_flush_pending(tx_desc_list);
    }
    return 0;
}

void nvic_default_setup(){

	bits_set(PPB_NVIC_ISER3_ADDR, 12, (1 << 12));
//	set_bit(PPB_NVIC_ICPR3_ADDR, 12, (1 << 12));

}

void nvic_enable_irq(unsigned int irq_number){

	unsigned int exception_number = irq_number;
	unsigned int register_index = exception_number / 32;
	unsigned int bit_position = exception_number % 32;

	if (register_index == 0){
		bits_set(PPB_NVIC_ISER0_ADDR, bit_position, (1 << bit_position));
	}
	else if (register_index == 1){
			bits_set(PPB_NVIC_ISER1_ADDR, bit_position, (1 << bit_position));
		}
	else if (register_index == 2){
				bits_set(PPB_NVIC_ISER2_ADDR, bit_position, (1 << bit_position));
			}
	else if (register_index == 3){
				bits_set(PPB_NVIC_ISER3_ADDR, bit_position, (1 << bit_position));
			}
//	set_bit(PPB_NVIC_ICPR3_ADDR, bit_position, (1 << bit_position));

}

void ETH_MAC_IRQHandler(void){
	printf("INTERRUPT HANDLER\n");
	static int cnt = 0;
	cnt++;
	count++;

	printf("cnt = %d, count = %d\n",cnt,count);

	mdio_reg_read_raw(0,0x1B,0,0xFFFF);
	if(mdio_wait_for_transfer() & 1){
		mdio_reg_read_raw(0,1,0,0xFFFF);
		if(mdio_wait_for_transfer() & 0x4){
			mdio_reg_write_raw(0,0,8,0xFF00,0x21);

		}
	}

}
void dump_test_buffers(ETH_TX_DESC *tx_desc_list,ETH_RX_DESC *rx_desc_list,ETH_TX_FRAME* Frame,ETH_RX_FRAME* rx_Frame){

		for(int i=0;i<3;i++){
			printf("Transmit descriptor[%d]\n",i);
			printf("TDES[0]: 0x%08X : 0x%08X\n",(unsigned int)&tx_desc_list->TDES0,tx_desc_list->TDES0);
			printf("TDES[1]: 0x%08X : 0x%08X\n",(unsigned int)&tx_desc_list->TDES1,tx_desc_list->TDES1);
			printf("TDES[2]: 0x%08X : 0x%08X\n",(unsigned int)&tx_desc_list->TDES2,tx_desc_list->TDES2);
			printf("TDES[3]: 0x%08X : 0x%08X\n",(unsigned int)&tx_desc_list->TDES3,tx_desc_list->TDES3);
			tx_desc_list++;
		}

		for(int i=0;i<3;i++){
			printf("Receive descriptor[%d]\n",i);
			printf("RDES[0]: 0x%08X : 0x%08X\n",(unsigned int)&rx_desc_list->RDES0,rx_desc_list->RDES0);
			printf("RDES[1]: 0x%08X : 0x%08X\n",(unsigned int)&rx_desc_list->RDES1,rx_desc_list->RDES1);
			printf("RDES[2]: 0x%08X : 0x%08X\n",(unsigned int)&rx_desc_list->RDES2,rx_desc_list->RDES2);
			printf("RDES[3]: 0x%08X : 0x%08X\n",(unsigned int)&rx_desc_list->RDES3,rx_desc_list->RDES3);
			rx_desc_list++;
		}

	printf("current tx_descriptor: 0x%08X\n",reg32_read(ETH0_REG_CURRENT_HOST_TRANSMIT_DESCRIPTOR_ADDR));
	printf("current rx_descriptor: 0x%08X\n",reg32_read(ETH0_REG_CURRENT_HOST_RECEIVE_DESCRIPTOR_ADDR));
	printf("current tx_buffer: 0x%08X\n",reg32_read(ETH0_REG_CURRENT_HOST_TRANSMIT_BUFFER_ADDRESS_ADDR));
	printf("current rx_buffer: 0x%08X\n",reg32_read(ETH0_REG_CURRENT_HOST_RECEIVE_BUFFER_ADDRESS_ADDR));

	printf("tx_interrupt: 0x%08X\n",bit_get(ETH0_REG_STATUS_ADDR,0));
	printf("rx_interrupt: 0x%08X\n",bit_get(ETH0_REG_STATUS_ADDR,6));
	printf("tx_buffer_unavailable: 0x%08X\n",bit_get(ETH0_REG_STATUS_ADDR,2));
	printf("bus interrupt: 0x%08X\n",bit_get(ETH0_REG_STATUS_ADDR,13));
	printf("error bits interrupt: 0x%08X\n",field_read(ETH0_REG_STATUS_ADDR,23,0x03800000));
	printf("Tx_descriptor_base_address; 0x%08X \n",reg32_read(ETH0_REG_TRANSMIT_DESCRIPTOR_LIST_ADDRESS_ADDR));
	printf("Rx_descriptor_base_address; 0x%08X \n",reg32_read(ETH0_REG_RECEIVE_DESCRIPTOR_LIST_ADDRESS_ADDR));
}

// Byte-compares the frame that was transmitted against the frame
int verify_loopback_data(ETH_TX_FRAME *Frame, ETH_RX_FRAME *rx_Frame, unsigned int frame_size, const char* test_label){
	unsigned char* frame_ptr = (unsigned char*)Frame;
	unsigned char* rx_frame_ptr = (unsigned char*)rx_Frame;
	int pass = 1;
	unsigned int mismatches_reported = 0;
	unsigned int total_mismatches = 0;

	for(unsigned int i=0; i<frame_size; i++){
		if(frame_ptr[i] != rx_frame_ptr[i]){
			pass = 0;
			total_mismatches++;
			if(mismatches_reported < 10){ /* cap the log spam on a real failure */
				printf("  byte[%u] mismatch: sent 0x%02X, received 0x%02X\n",
						i, frame_ptr[i], rx_frame_ptr[i]);
				mismatches_reported++;
			}
		}
	}

	if(pass){
		printf("TEST CASE: PASS - %u bytes verified through %s loopback\n", frame_size, test_label);
	}
	else{
		printf("TEST CASE: FAIL - %s loopback data mismatch (%u/%u bytes wrong)\n",
			test_label, total_mismatches, frame_size);
	}
	return pass;
}

// Polls for TX/RX completion with a bounded number of retries, so a
// failed test reports FAIL instead of hanging forever.
int wait_for_transmit_complete(void){
	for(unsigned int attempt = 0; attempt < LOOPBACK_MAX_RETRIES; attempt++){
		if(tx_irq_flag_is_set() != 0){
			return 1;
		}
		printf("  tx wait %u: STATUS=0x%08X\n", attempt, reg32_read(ETH0_REG_STATUS_ADDR));
		tx_dma_poll_demand();
		tx_dma_start();
	}
	return 0;
}

int wait_for_receive_complete(void){
	for(unsigned int attempt = 0; attempt < LOOPBACK_MAX_RETRIES; attempt++){
		if(rx_irq_flag_is_set() != 0){
			return 1;
		}
		printf("  rx wait %u: STATUS=0x%08X\n", attempt, reg32_read(ETH0_REG_STATUS_ADDR));
		rx_dma_poll_demand();
		rx_dma_start();
	}
	return 0;
}

// Self-contained internal-loopback test case for the Ethernet MAC.
int run_mac_loopback_test(ETH_TX_FRAME* Frame, ETH_RX_FRAME* rx_Frame, ETH_TX_DESC* tx_desc_list, ETH_RX_DESC* rx_desc_list){
	unsigned int frame_size = (unsigned int)sizeof(ETH_TX_FRAME);
	unsigned char l_f;

	memset(Frame, 0x00, sizeof(ETH_TX_FRAME));
	memset(rx_Frame, 0xFF, sizeof(ETH_RX_FRAME)); // fill RX buffer with junk so a "pass" has to be real
	memset(tx_desc_list, 0, sizeof(ETH_TX_DESC)*4);
	memset(rx_desc_list, 0, sizeof(ETH_RX_DESC)*4);

	// Internal loopback: TX data is routed straight back to RX inside the
	// MAC, no PHY or cable needed.
	mac_loopback_enable();
	printf("enter the duplex mode l/f:");
	scanf("%c",&l_f);
	mac_apply_config(l_f, 'f', 'n');
//	mac_apply_config('l', 'f', 'n');

	// Set up the RX side first so it's ready before we send anything.
	rx_set_store_forward_mode();
	rx_dma_stop();
	rx_set_desc_list_base(rx_desc_list);
	rx_desc_build_ring(rx_desc_list, (unsigned int*)rx_Frame, frame_size);
	rx_dma_start();

	// Build a test frame with a known pattern and send it.
	tx_fill_test_frame(Frame);

	printf("  tx data: dest=%02X:%02X:%02X:%02X:%02X:%02X src=%02X:%02X:%02X:%02X:%02X:%02X len/type=%02X%02X payload[0..3]=%02X %02X %02X %02X (frame_size=%u bytes)\n",
		Frame->dest_addr[0],Frame->dest_addr[1],Frame->dest_addr[2],Frame->dest_addr[3],Frame->dest_addr[4],Frame->dest_addr[5],
		Frame->sour_addr[0],Frame->sour_addr[1],Frame->sour_addr[2],Frame->sour_addr[3],Frame->sour_addr[4],Frame->sour_addr[5],
		Frame->len_type[0],Frame->len_type[1],
		Frame->payload[0],Frame->payload[1],Frame->payload[2],Frame->payload[3],
		frame_size);

	tx_set_store_forward_mode();
	tx_dma_stop();
	tx_set_desc_list_base(tx_desc_list);
	tx_desc_mark_first_seg(tx_desc_list);
	tx_desc_build_ring(tx_desc_list, (unsigned int*)Frame, frame_size);

	tx_dma_poll_demand();
	tx_dma_start();

	if(!wait_for_transmit_complete()){
		printf("TEST CASE: FAIL - transmit did not complete (MAC-only loopback)\n");
		return 0;
	}

	if(!wait_for_receive_complete()){
		printf("TEST CASE: FAIL - no frame received back (MAC-only loopback)\n");
		return 0;
	}

	printf("  rx data: dest=%02X:%02X:%02X:%02X:%02X:%02X src=%02X:%02X:%02X:%02X:%02X:%02X len/type=%02X%02X payload[0..3]=%02X %02X %02X %02X\n",
		rx_Frame->dest_addr[0],rx_Frame->dest_addr[1],rx_Frame->dest_addr[2],rx_Frame->dest_addr[3],rx_Frame->dest_addr[4],rx_Frame->dest_addr[5],
		rx_Frame->sour_addr[0],rx_Frame->sour_addr[1],rx_Frame->sour_addr[2],rx_Frame->sour_addr[3],rx_Frame->sour_addr[4],rx_Frame->sour_addr[5],
		rx_Frame->len_type[0],rx_Frame->len_type[1],
		rx_Frame->payload[0],rx_Frame->payload[1],rx_Frame->payload[2],rx_Frame->payload[3]);

	return verify_loopback_data(Frame, rx_Frame, frame_size, "MAC-only (LM bit)");
}

// Self-contained PHY-loopback test case for the Ethernet MAC +
int run_phy_loopback_test(ETH_TX_FRAME* Frame, ETH_RX_FRAME* rx_Frame, ETH_TX_DESC* tx_desc_list, ETH_RX_DESC* rx_desc_list){
	unsigned int frame_size = (unsigned int)sizeof(ETH_TX_FRAME);

	printf("  ----- PHY loopback (BMCR.14) -----\n");

	memset(Frame, 0x00, sizeof(ETH_TX_FRAME));
	memset(rx_Frame, 0xFF, sizeof(ETH_RX_FRAME)); // fill RX buffer with junk so a "pass" has to be real
	memset(tx_desc_list, 0, sizeof(ETH_TX_DESC)*4);
	memset(rx_desc_list, 0, sizeof(ETH_RX_DESC)*4);

	// Make sure the MAC's own internal loopback bit is off, otherwise
	// frames would loop back inside the MAC and never really reach the PHY.
	mac_loopback_disable();
	printf("  mac loopback bit = %u (should be 0)\n", mac_loopback_is_enabled());

	mac_apply_config('l', 'f', 'n');     // 10Mbps, full duplex - matches phy_enable_loopback()'s BMCR settings
	phy_enable_loopback(PHY_ADDR); // BMCR bit14: loop the signal back inside the PHY itself

	// Read BMCR back over MDIO to confirm the write actually took effect.
	unsigned int bmcr_readback = phy_read_register(PHY_ADDR, PHY_REG_BMCR);
	printf("  phy BMCR read back = 0x%04X (loopback bit %s)\n",
		bmcr_readback, (bmcr_readback & PHY_BMCR_LOOPBACK_Msk) ? "set" : "NOT set");

	phy_print_all_registers(PHY_ADDR);

	// Short delay so the PHY has time to settle into loopback mode before
	// we send the first frame.
	for(volatile unsigned int settle = 0; settle < 150000u; settle++){
		__NOP();
	}

	// Set up the RX side first so it's ready before we send anything.
	rx_set_store_forward_mode();
	rx_dma_stop();
	rx_set_desc_list_base(rx_desc_list);
	rx_desc_build_ring(rx_desc_list, (unsigned int*)rx_Frame, frame_size);
	rx_dma_start();

	// Build a test frame with a known pattern and send it.
	tx_fill_test_frame(Frame);

	printf("  tx data: dest=%02X:%02X:%02X:%02X:%02X:%02X src=%02X:%02X:%02X:%02X:%02X:%02X len/type=%02X%02X payload[0..3]=%02X %02X %02X %02X (frame_size=%u bytes)\n",
		Frame->dest_addr[0],Frame->dest_addr[1],Frame->dest_addr[2],Frame->dest_addr[3],Frame->dest_addr[4],Frame->dest_addr[5],
		Frame->sour_addr[0],Frame->sour_addr[1],Frame->sour_addr[2],Frame->sour_addr[3],Frame->sour_addr[4],Frame->sour_addr[5],
		Frame->len_type[0],Frame->len_type[1],
		Frame->payload[0],Frame->payload[1],Frame->payload[2],Frame->payload[3],
		frame_size);

	tx_set_store_forward_mode();
	tx_dma_stop();
	tx_set_desc_list_base(tx_desc_list);
	tx_desc_mark_first_seg(tx_desc_list);
	tx_desc_build_ring(tx_desc_list, (unsigned int*)Frame, frame_size);

	tx_dma_poll_demand();
	tx_dma_start();

	if(!wait_for_transmit_complete()){
		printf("TEST CASE: FAIL - transmit did not complete (PHY loopback)\n");
		phy_disable_loopback(PHY_ADDR);
		return 0;
	}

	if(!wait_for_receive_complete()){
		printf("TEST CASE: FAIL - no frame received back (PHY loopback) - check PHY_ADDR and MDIO wiring\n");
		phy_disable_loopback(PHY_ADDR);
		return 0;
	}

	printf("  rx data: dest=%02X:%02X:%02X:%02X:%02X:%02X src=%02X:%02X:%02X:%02X:%02X:%02X len/type=%02X%02X payload[0..3]=%02X %02X %02X %02X\n",
		rx_Frame->dest_addr[0],rx_Frame->dest_addr[1],rx_Frame->dest_addr[2],rx_Frame->dest_addr[3],rx_Frame->dest_addr[4],rx_Frame->dest_addr[5],
		rx_Frame->sour_addr[0],rx_Frame->sour_addr[1],rx_Frame->sour_addr[2],rx_Frame->sour_addr[3],rx_Frame->sour_addr[4],rx_Frame->sour_addr[5],
		rx_Frame->len_type[0],rx_Frame->len_type[1],
		rx_Frame->payload[0],rx_Frame->payload[1],rx_Frame->payload[2],rx_Frame->payload[3]);

	phy_disable_loopback(PHY_ADDR); // take the PHY back out of loopback before returning

	return verify_loopback_data(Frame, rx_Frame, frame_size, "PHY (BMCR.14)");
}

// Powers the PHY down and re-runs the same test, expecting it to now FAIL.
// This proves the earlier PHY-loopback PASS really went through the PHY,
// and wasn't secretly just the MAC's own internal loopback bit again.
int run_phy_powerdown_sanity_check(ETH_TX_FRAME* Frame, ETH_RX_FRAME* rx_Frame, ETH_TX_DESC* tx_desc_list, ETH_RX_DESC* rx_desc_list){
	unsigned int frame_size = (unsigned int)sizeof(ETH_TX_FRAME);

	printf("\n  ----- Sanity check: PHY powered down, should get no data back -----\n");

	memset(Frame, 0x00, sizeof(ETH_TX_FRAME));
	memset(rx_Frame, 0xFF, sizeof(ETH_RX_FRAME));
	memset(tx_desc_list, 0, sizeof(ETH_TX_DESC)*4);
	memset(rx_desc_list, 0, sizeof(ETH_RX_DESC)*4);

	mac_loopback_disable();          // same setup as the real PHY loopback test
	mac_apply_config('l', 'f', 'n');

	unsigned int bmcr = phy_read_register(PHY_ADDR, PHY_REG_BMCR);
	bmcr &= ~PHY_BMCR_LOOPBACK_Msk;  // make sure loopback itself is off
	bmcr |= PHY_BMCR_POWERDOWN_Msk;  // bit11: power the PHY down completely
	phy_write_register(PHY_ADDR, PHY_REG_BMCR, bmcr);

	unsigned int readback = phy_read_register(PHY_ADDR, PHY_REG_BMCR);
	printf("  phy BMCR read back = 0x%04X (powerdown=%u, loopback=%u)\n",
		readback,
		(readback & PHY_BMCR_POWERDOWN_Msk) ? 1u : 0u,
		(readback & PHY_BMCR_LOOPBACK_Msk)  ? 1u : 0u);

	rx_set_store_forward_mode();
	rx_dma_stop();
	rx_set_desc_list_base(rx_desc_list);
	rx_desc_build_ring(rx_desc_list, (unsigned int*)rx_Frame, frame_size);
	rx_dma_start();

	tx_fill_test_frame(Frame);
	tx_set_store_forward_mode();
	tx_dma_stop();
	tx_set_desc_list_base(tx_desc_list);
	tx_desc_mark_first_seg(tx_desc_list);
	tx_desc_build_ring(tx_desc_list, (unsigned int*)Frame, frame_size);
	tx_dma_poll_demand();
	tx_dma_start();

	wait_for_transmit_complete();  // TX can still complete on its own even with the PHY down,
	                                // so we only check RX for this test.

	int rx_arrived = wait_for_receive_complete();

	// Power the PHY back up so it's not left disabled for anything after this.
	bmcr = phy_read_register(PHY_ADDR, PHY_REG_BMCR);
	bmcr &= ~PHY_BMCR_POWERDOWN_Msk;
	phy_write_register(PHY_ADDR, PHY_REG_BMCR, bmcr);

	if(!rx_arrived){
		printf("SANITY CHECK: PASS - no data came back with the PHY powered down, so the earlier PHY loopback result was genuine.\n");
		return 1;
	}
	else{
		printf("SANITY CHECK: FAIL - data still came back with the PHY powered down. The earlier PHY loopback PASS is suspect - check the MAC loopback bit and PHY_ADDR/MDIO wiring.\n");
		return 0;
	}
}

int main(void)
{
    unsigned int flag1 = 0;

    /* 1. Hardware Initialization */
    clock_init_pll(47, 1, 1);
    uart_console_init();
    printf("PLL Clocks and UART are enabled\n");

    gpio_init_eth_pins();
    printf("GPIO pins enabled for Ethernet functionality\n");

    phy_reset_and_init('r');
    printf("Phy configured\n");

    eth_select_rmii_interface();
    printf("RMII Configured\n");

    mdio_select_input_pin();
    printf("MDIO pins Configured\n");

    eth_power_clock_init();
    printf("Enabled Ethernet and MDIO clocks\n");

    time_stamp_init();
    printf("Enabled time stamp\n");

    bits_set(ETH0_REG_MAC_CONFIGURATION_ADDR, 7, 0x80);
    printf("Enabled CRC\n");

    eth_configure_dma_bus('r', 'r', 3, 0x0);
    printf("DMA configured\n");

    /* 2. Buffer & Descriptor Initialization */
    ETH_TX_DESC*  tx_desc_list = NULL;
    ETH_RX_DESC*  rx_desc_list = NULL;
    ETH_TX_FRAME* Frame = NULL;
    ETH_RX_FRAME* rx_Frame = NULL;

    flag1 = tx_rx_buff_init(&tx_desc_list, &rx_desc_list, &Frame, &rx_Frame);
    if (flag1 == 0) {
        printf("Buffer initialized\n");
    } else {
        printf("Buffer initialization failed!\n");
        return 0;
    }

    /* 3. Automation Handshake */
    printf("\r\n========================================\r\n");
    printf("XMC4700 Ethernet Test Harness Initialized.\r\n");
    printf("Waiting for commands from Automation GUI...\r\n");
    printf("========================================\r\n");

    /* 4. Command Interpreter Loop */
    char cmd_buffer[64];
    uint8_t idx = 0;

    while (1)
    {
        if (uart_has_data())
        {
            char c = uart_read_char();

            if (c == '\r' || c == '\n')
            {
                if (idx > 0)
                {
                    cmd_buffer[idx] = '\0';

                    if (strncmp(cmd_buffer, "RUN ", 4) == 0)
                    {
                        char *test_name = &cmd_buffer[4];
                        printf("[TARGET] Executing: %s\r\n", test_name);

                        bool result = false;

                        if (strcmp(test_name, "ETH_MAC_10_HALF") == 0) {
                            result = ETH_RunTestCase(ETH_TEST_MAC_10M_HALF, Frame, rx_Frame, tx_desc_list, rx_desc_list);
                        } else if (strcmp(test_name, "ETH_MAC_10_FULL") == 0) {
                            result = ETH_RunTestCase(ETH_TEST_MAC_10M_FULL, Frame, rx_Frame, tx_desc_list, rx_desc_list);
                        } else if (strcmp(test_name, "ETH_MAC_100_HALF") == 0) {
                            result = ETH_RunTestCase(ETH_TEST_MAC_100M_HALF, Frame, rx_Frame, tx_desc_list, rx_desc_list);
                        } else if (strcmp(test_name, "ETH_MAC_100_FULL") == 0) {
                            result = ETH_RunTestCase(ETH_TEST_MAC_100M_FULL, Frame, rx_Frame, tx_desc_list, rx_desc_list);
                        } else if (strcmp(test_name, "ETH_PHY_10_HALF") == 0) {
                            result = ETH_RunTestCase(ETH_TEST_PHY_10M_HALF, Frame, rx_Frame, tx_desc_list, rx_desc_list);
                        } else if (strcmp(test_name, "ETH_PHY_10_FULL") == 0) {
                            result = ETH_RunTestCase(ETH_TEST_PHY_10M_FULL, Frame, rx_Frame, tx_desc_list, rx_desc_list);
                        } else if (strcmp(test_name, "ETH_PHY_100_HALF") == 0) {
                            result = ETH_RunTestCase(ETH_TEST_PHY_100M_HALF, Frame, rx_Frame, tx_desc_list, rx_desc_list);
                        } else if (strcmp(test_name, "ETH_PHY_100_FULL") == 0) {
                            result = ETH_RunTestCase(ETH_TEST_PHY_100M_FULL, Frame, rx_Frame, tx_desc_list, rx_desc_list);
                        } else {
                            printf("[TARGET] UNKNOWN_TEST\r\n");
                        }

                        /* Exact handshake string expected by Python GUI */
                        if (result) {
                            printf("STATUS: PASS\r\n");
                        } else {
                            printf("STATUS: FAIL\r\n");
                        }
                    }

                    idx = 0;
                }
            }
            else
            {
                if (idx < sizeof(cmd_buffer) - 1)
                {
                    cmd_buffer[idx++] = c;
                }
            }
        }
    }

    return 0;
}

