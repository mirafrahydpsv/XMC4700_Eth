/*-----------------------ETH REGISTERS----------------------------*/

#define   ETH0_REG_MAC_CONFIGURATION_ADDR                  0x5000C000
#define   ETH0_REG_MAC_FRAME_FILTER_ADDR                   0x5000C004
#define   ETH0_REG_HASH_TABLE_HIGH_ADDR                    0x5000C008
#define   ETH0_REG_HASH_TABLE_LOW_ADDR                     0x5000C00C
#define   ETH0_REG_GMII_ADDRESS_ADDR                       0x5000C010                                   
#define   ETH0_REG_GMII_DATA_ADDR                          0x5000C014                                      
#define   ETH0_REG_FLOW_CONTROL_ADDR                       0x5000C018                                  
#define   ETH0_REG_VLAN_TAG_ADDR                           0x5000C01C 
#define   ETH0_REG_VERSION_ADDR                            0x5000C020                                      
#define   ETH0_REG_DEBUG_ADDR                              0x5000C024                                        
#define   ETH0_REG_REMOTE_WAKE_UP_FRAME_FILTER_ADDR        0x5000C028                  
#define   ETH0_REG_PMT_CONTROL_STATUS_ADDR                 0x5000C02C                        
#define   ETH0_REG_RESERVED [2]
#define   ETH0_REG_INTERRUPT_STATUS_ADDR                   0x5000C038                                     
#define   ETH0_REG_INTERRUPT_MASK_ADDR                     0x5000C03C                               
#define   ETH0_REG_MAC_ADDRESS0_HIGH_ADDR                  0x5000C040                             
#define   ETH0_REG_MAC_ADDRESS0_LOW_ADDR                   0x5000C044                            
#define   ETH0_REG_MAC_ADDRESS1_HIGH_ADDR                  0x5000C048                             
#define   ETH0_REG_MAC_ADDRESS1_LOW_ADDR                   0x5000C04C                             
#define   ETH0_REG_MAC_ADDRESS2_HIGH_ADDR                  0x5000C050                          
#define   ETH0_REG_MAC_ADDRESS2_LOW_ADDR                   0x5000C054                             
#define   ETH0_REG_MAC_ADDRESS3_HIGH_ADDR                  0x5000C058                            
#define   ETH0_REG_MAC_ADDRESS3_LOW_ADDR                   0x5000C05C                              
#define   ETH0_REG_RESERVED1 [40]
#define   ETH0_REG_MMC_CONTROL_ADDR                        0x5000C100                                   
#define   ETH0_REG_MMC_RECEIVE_INTERRUPT_ADDR              0x5000C104                         
#define   ETH0_REG_MMC_TRANSMIT_INTERRUPT_ADDR             0x5000C108                        
#define   ETH0_REG_MMC_RECEIVE_INTERRUPT_MASK_ADDR         0x5000C10C                   
#define   ETH0_REG_MMC_TRANSMIT_INTERRUPT_MASK_ADDR        0x5000C110                  
#define   ETH0_REG_TX_OCTET_COUNT_GOOD_BAD_ADDR            0x5000C114
#define   ETH0_REG_TX_FRAME_COUNT_GOOD_BAD_ADDR            0x5000C118
#define   ETH0_REG_TX_BROADCAST_FRAMES_GOOD_ADDR           0x5000C11C
#define   ETH0_REG_TX_MULTICAST_FRAMES_GOOD_ADDR           0x5000C120
#define   ETH0_REG_TX_64OCTETS_FRAMES_GOOD_BAD_ADDR        0x5000C124
#define   ETH0_REG_TX_65TO127OCTETS_FRAMES_GOOD_BAD_ADDR   0x5000C128
#define   ETH0_REG_TX_128TO255OCTETS_FRAMES_GOOD_BAD_ADDR  0x5000C12C
#define   ETH0_REG_TX_256TO511OCTETS_FRAMES_GOOD_BAD_ADDR  0x5000C130
#define   ETH0_REG_TX_512TO1023OCTETS_FRAMES_GOOD_BAD_ADDR  0x5000C134
#define   ETH0_REG_TX_1024TOMAXOCTETS_FRAMES_GOOD_BAD_ADDR  0x5000C138
#define   ETH0_REG_TX_UNICAST_FRAMES_GOOD_BAD_ADDR         0x5000C13C
#define   ETH0_REG_TX_MULTICAST_FRAMES_GOOD_BAD_ADDR       0x5000C140
#define   ETH0_REG_TX_BROADCAST_FRAMES_GOOD_BAD_ADDR       0x5000C144
#define   ETH0_REG_TX_UNDERFLOW_ERROR_FRAMES_ADDR          0x5000C148
#define   ETH0_REG_TX_SINGLE_COLLISION_GOOD_FRAMES_ADDR    0x5000C14C
#define   ETH0_REG_TX_MULTIPLE_COLLISION_GOOD_FRAMES_ADDR  0x5000C150
#define   ETH0_REG_TX_DEFERRED_FRAMES_ADDR                 0x5000C154
#define   ETH0_REG_TX_LATE_COLLISION_FRAMES_ADDR           0x5000C158
#define   ETH0_REG_TX_EXCESSIVE_COLLISION_FRAMES_ADDR      0x5000C15C
#define   ETH0_REG_TX_CARRIER_ERROR_FRAMES_ADDR            0x5000C160
#define   ETH0_REG_TX_OCTET_COUNT_GOOD_ADDR                0x5000C164
#define   ETH0_REG_TX_FRAME_COUNT_GOOD_ADDR                0x5000C168
#define   ETH0_REG_TX_EXCESSIVE_DEFERRAL_ERROR_ADDR        0x5000C16C
#define   ETH0_REG_TX_PAUSE_FRAMES_ADDR                    0x5000C170
#define   ETH0_REG_TX_VLAN_FRAMES_GOOD_ADDR                0x5000C174
#define   ETH0_REG_TX_OSIZE_FRAMES_GOOD_ADDR               0x5000C178
#define   ETH0_REG_RESERVED2
#define   ETH0_REG_RX_FRAMES_COUNT_GOOD_BAD_ADDR           0x5000C180
#define   ETH0_REG_RX_OCTET_COUNT_GOOD_BAD_ADDR            0x5000C184
#define   ETH0_REG_RX_OCTET_COUNT_GOOD_ADDR                0x5000C188
#define   ETH0_REG_RX_BROADCAST_FRAMES_GOOD_ADDR           0x5000C18C
#define   ETH0_REG_RX_MULTICAST_FRAMES_GOOD_ADDR           0x5000C190
#define   ETH0_REG_RX_CRC_ERROR_FRAMES_ADDR                0x5000C194
#define   ETH0_REG_RX_ALIGNMENT_ERROR_FRAMES_ADDR          0x5000C198
#define   ETH0_REG_RX_RUNT_ERROR_FRAMES_ADDR               0x5000C19C
#define   ETH0_REG_RX_JABBER_ERROR_FRAMES_ADDR             0x5000C1A0
#define   ETH0_REG_RX_UNDERSIZE_FRAMES_GOOD_ADDR           0x5000C1A4
#define   ETH0_REG_RX_OVERSIZE_FRAMES_GOOD_ADDR            0x5000C1A8
#define   ETH0_REG_RX_64OCTETS_FRAMES_GOOD_BAD_ADDR        0x5000C1AC
#define   ETH0_REG_RX_65TO127OCTETS_FRAMES_GOOD_BAD_ADDR   0x5000C1B0
#define   ETH0_REG_RX_128TO255OCTETS_FRAMES_GOOD_BAD_ADDR  0x5000C1B4
#define   ETH0_REG_RX_256TO511OCTETS_FRAMES_GOOD_BAD_ADDR  0x5000C1B8
#define   ETH0_REG_RX_512TO1023OCTETS_FRAMES_GOOD_BAD_ADDR  0x5000C1BC
#define   ETH0_REG_RX_1024TOMAXOCTETS_FRAMES_GOOD_BAD_ADDR  0x5000C1C0
#define   ETH0_REG_RX_UNICAST_FRAMES_GOOD_ADDR             0x5000C1C4
#define   ETH0_REG_RX_LENGTH_ERROR_FRAMES_ADDR             0x5000C1C8
#define   ETH0_REG_RX_OUT_OF_RANGE_TYPE_FRAMES_ADDR        0x5000C1CC
#define   ETH0_REG_RX_PAUSE_FRAMES_ADDR                    0x5000C1D0
#define   ETH0_REG_RX_FIFO_OVERFLOW_FRAMES_ADDR            0x5000C1D4
#define   ETH0_REG_RX_VLAN_FRAMES_GOOD_BAD_ADDR            0x5000C1D8
#define   ETH0_REG_RX_WATCHDOG_ERROR_FRAMES_ADDR           0x5000C1DC
#define   ETH0_REG_RX_RECEIVE_ERROR_FRAMES_ADDR            0x5000C1E0
#define   ETH0_REG_RX_CONTROL_FRAMES_GOOD_ADDR             0x5000C1E4
#define   ETH0_REG_RESERVED3 [6]
#define   ETH0_REG_MMC_IPC_RECEIVE_INTERRUPT_MASK_ADDR     0x5000C200
#define   ETH0_REG_RESERVED4
#define   ETH0_REG_MMC_IPC_RECEIVE_INTERRUPT_ADDR          0x5000C208
#define   ETH0_REG_RESERVED5
#define   ETH0_REG_RXIPV4_GOOD_FRAMES_ADDR                 0x5000C210
#define   ETH0_REG_RXIPV4_HEADER_ERROR_FRAMES_ADDR         0x5000C214
#define   ETH0_REG_RXIPV4_NO_PAYLOAD_FRAMES_ADDR           0x5000C218
#define   ETH0_REG_RXIPV4_FRAGMENTED_FRAMES_ADDR           0x5000C21C
#define   ETH0_REG_RXIPV4_UDP_CHECKSUM_DISABLED_FRAMES_ADDR 0x5000C220
#define   ETH0_REG_RXIPV6_GOOD_FRAMES_ADDR                 0x5000C224
#define   ETH0_REG_RXIPV6_HEADER_ERROR_FRAMES_ADDR         0x5000C228
#define   ETH0_REG_RXIPV6_NO_PAYLOAD_FRAMES_ADDR           0x5000C22C
#define   ETH0_REG_RXUDP_GOOD_FRAMES_ADDR                  0x5000C230
#define   ETH0_REG_RXUDP_ERROR_FRAMES_ADDR                 0x5000C234
#define   ETH0_REG_RXTCP_GOOD_FRAMES_ADDR                  0x5000C238
#define   ETH0_REG_RXTCP_ERROR_FRAMES_ADDR                 0x5000C23C
#define   ETH0_REG_RXICMP_GOOD_FRAMES_ADDR                 0x5000C240
#define   ETH0_REG_RXICMP_ERROR_FRAMES_ADDR                0x5000C244
#define   ETH0_REG_RESERVED6 [2]
#define   ETH0_REG_RXIPV4_GOOD_OCTETS_ADDR                 0x5000C250
#define   ETH0_REG_RXIPV4_HEADER_ERROR_OCTETS_ADDR         0x5000C254
#define   ETH0_REG_RXIPV4_NO_PAYLOAD_OCTETS_ADDR           0x5000C258
#define   ETH0_REG_RXIPV4_FRAGMENTED_OCTETS_ADDR           0x5000C25C
#define   ETH0_REG_RXIPV4_UDP_CHECKSUM_DISABLE_OCTETS_ADDR  0x5000C260
#define   ETH0_REG_RXIPV6_GOOD_OCTETS_ADDR                 0x5000C264
#define   ETH0_REG_RXIPV6_HEADER_ERROR_OCTETS_ADDR         0x5000C268
#define   ETH0_REG_RXIPV6_NO_PAYLOAD_OCTETS_ADDR           0x5000C26C
#define   ETH0_REG_RXUDP_GOOD_OCTETS_ADDR                  0x5000C270
#define   ETH0_REG_RXUDP_ERROR_OCTETS_ADDR                 0x5000C274
#define   ETH0_REG_RXTCP_GOOD_OCTETS_ADDR                  0x5000C278
#define   ETH0_REG_RXTCP_ERROR_OCTETS_ADDR                 0x5000C27C
#define   ETH0_REG_RXICMP_GOOD_OCTETS_ADDR                 0x5000C280
#define   ETH0_REG_RXICMP_ERROR_OCTETS_ADDR                0x5000C284
#define   ETH0_REG_RESERVED7 [286]
#define   ETH0_REG_TIMESTAMP_CONTROL_ADDR                  0x5000C700
#define   ETH0_REG_SUB_SECOND_INCREMENT_ADDR               0x5000C704
#define   ETH0_REG_SYSTEM_TIME_SECONDS_ADDR                0x5000C708
#define   ETH0_REG_SYSTEM_TIME_NANOSECONDS_ADDR            0x5000C70C
#define   ETH0_REG_SYSTEM_TIME_SECONDS_UPDATE_ADDR         0x5000C710
#define   ETH0_REG_SYSTEM_TIME_NANOSECONDS_UPDATE_ADDR     0x5000C714
#define   ETH0_REG_TIMESTAMP_ADDEND_ADDR                   0x5000C718
#define   ETH0_REG_TARGET_TIME_SECONDS_ADDR                0x5000C71C
#define   ETH0_REG_TARGET_TIME_NANOSECONDS_ADDR            0x5000C720
#define   ETH0_REG_SYSTEM_TIME_HIGHER_WORD_SECONDS_ADDR    0x5000C724
#define   ETH0_REG_TIMESTAMP_STATUS_ADDR                   0x5000C728
#define   ETH0_REG_RESERVED8 [565]
#define   ETH0_REG_BUS_MODE_ADDR                           0x5000D000
#define   ETH0_REG_TRANSMIT_POLL_DEMAND_ADDR               0x5000D004
#define   ETH0_REG_RECEIVE_POLL_DEMAND_ADDR                0x5000D008
#define   ETH0_REG_RECEIVE_DESCRIPTOR_LIST_ADDRESS_ADDR    0x5000D00C
#define   ETH0_REG_TRANSMIT_DESCRIPTOR_LIST_ADDRESS_ADDR   0x5000D010
#define   ETH0_REG_STATUS_ADDR                             0x5000D014
#define   ETH0_REG_OPERATION_MODE_ADDR                     0x5000D018
#define   ETH0_REG_INTERRUPT_ENABLE_ADDR                   0x5000D01C
#define   ETH0_REG_MISSED_FRAME_AND_BUFFER_OVERFLOW_COUNTER_ADDR  0x5000D020
#define   ETH0_REG_RECEIVE_INTERRUPT_WATCHDOG_TIMER_ADDR     0x5000D024
#define   ETH0_REG_RESERVED9
#define   ETH0_REG_AHB_STATUS_ADDR                           0x5000D02C
#define   ETH0_REG_RESERVED10 [6]
#define   ETH0_REG_CURRENT_HOST_TRANSMIT_DESCRIPTOR_ADDR     0x5000D048
#define   ETH0_REG_CURRENT_HOST_RECEIVE_DESCRIPTOR_ADDR      0x5000D04C
#define   ETH0_REG_CURRENT_HOST_TRANSMIT_BUFFER_ADDRESS_ADDR         0x5000D050
#define   ETH0_REG_CURRENT_HOST_RECEIVE_BUFFER_ADDRESS_ADDR  0x5000D054
#define   ETH0_REG_HW_FEATURE_ADDR                           0x5000D058








/*--------------------------SCU REGISTERS-------------------------*/

#define  SCU_REG_CLKSTAT_ADDR              0x50004600
#define  SCU_REG_CLKSET_ADDR               0x50004604
#define  SCU_REG_CLKCLR_ADDR               0x50004608
#define  SCU_REG_SYSCLKCR_ADDR             0x5000460C
#define  SCU_REG_CPUCLKCR_ADDR             0x50004610
#define  SCU_REG_PBCLKCR_ADDR              0x50004614
#define  SCU_REG_USBCLKCR_ADDR             0x50004618
#define  SCU_REG_EBUCLKCR_ADDR             0x5000461C
#define  SCU_REG_CCUCLKCR_ADDR             0x50004620
#define  SCU_REG_WDTCLKCR_ADDR             0x50004624
#define  SCU_REG_EXTCLKCR_ADDR             0x50004628
#define  SCU_REG_MLINKCLKCR_ADDR           0x5000462C
#define  SCU_REG_SLEEPCR_ADDR              0x50004630
#define  SCU_REG_DSLEEPCR_ADDR             0x50004634
#define  SCU_REG_RESERVED [2];
#define  SCU_REG_CGATSTAT0_ADDR            0x50004640
#define  SCU_REG_CGATSET0_ADDR             0x50004644
#define  SCU_REG_CGATCLR0_ADDR             0x50004648
#define  SCU_REG_CGATSTAT1_ADDR            0x5000464C
#define  SCU_REG_CGATSET1_ADDR             0x50004650
#define  SCU_REG_CGATCLR1_ADDR             0x50004654
#define  SCU_REG_CGATSTAT2_ADDR            0x50004658
#define  SCU_REG_CGATSET2_ADDR             0x5000465C
#define  SCU_REG_CGATCLR2_ADDR             0x50004660
#define  SCU_REG_CGATSTAT3_ADDR            0x50004664
#define  SCU_REG_CGATSET3_ADDR             0x50004668
#define  SCU_REG_CGATCLR3_ADDR             0x5000466C




#define  SCU_REG_RSTSTAT_ADDR                     0x50004400
#define  SCU_REG_RSTSET_ADDR                      0x50004404
#define  SCU_REG_RSTCLR_ADDR                      0x50004408
#define  SCU_REG_PRSTAT0_ADDR                     0x5000440C
#define  SCU_REG_PRSET0_ADDR                      0x50004410  
#define  SCU_REG_PRCLR0_ADDR                      0x50004414 
#define  SCU_REG_PRSTAT1_ADDR                     0x50004418 
#define  SCU_REG_PRSET1_ADDR                      0x5000441C  
#define  SCU_REG_PRCLR1_ADDR                      0x50004420 
#define  SCU_REG_PRSTAT2_ADDR                     0x50004424 
#define  SCU_REG_PRSET2_ADDR                      0x50004428  
#define  SCU_REG_PRCLR2_ADDR                      0x5000442C 
#define  SCU_REG_PRSTAT3_ADDR                     0x50004430 
#define  SCU_REG_PRSET3_ADDR                      0x50004434  
#define  SCU_REG_PRCLR3_ADDR                      0x50004438 


#define  SCU_REG_CON_ADDR                         0x50004040


//SCU PLL
   #define SCU_PLL_PLLSTAT_ADDR		0x50004710  /* PLL Status Register                                    */
   #define SCU_PLL_PLLCON0_ADDR 	0x50004714  /* sPLL Configuration 0 Register                           */
   #define SCU_PLL_PLLCON1_ADDR 	0x50004718  /* PLL Configuration 1 Register                           */
   #define SCU_PLL_PLLCON2_ADDR 	0x5000471C  /* PLL Configuration 2 Register                           */
   #define SCU_PLL_USBPLLSTAT_ADDR 	0x50004720  /* USB PLL Status Register                                */
   #define SCU_PLL_USBPLLCON_ADDR 	0x50004724  /* USB PLL Configuration Register                         */
   #define SCU_PLL_CLKMXSTAT_ADDR 	0x5000473    /* Clock Multiplexing Status Register                     */

//SCU OSC
  #define SCU_OSC_OSCHPSTAT_ADDR      0x50004700    /* OSC_HP Status Register                                 */
  #define SCU_OSC_OSCHPCTRL_ADDR      0x50004704    /* OSC_HP Control Register                                */
  #define SCU_OSC_CLKCALCONST_ADDR    0x5000470C    /* Clock Calibration Constant Register                    */

//port 15
  #define PORT15_OUT_ADDR    	0x48028F00    	/*Port 15 Output Register                                */
  #define PORT15_OMR_ADDR    	0x48028F04    	/*Port 15 Output Modification Register                   */
  #define PORT15_IOCR0_ADDR     0x48028F10      /*Port 15 Input/Output Control Register 0                */
  #define PORT15_IOCR4_ADDR     0x48028F14    	/*Port 15 Input/Output Control Register 4                */
  #define PORT15_IOCR8_ADDR     0x48028F18    	/*Port 15 Input/Output Control Register 8                */
  #define PORT15_IOCR12_ADDR    0x48028F1C      /*Port 15 Input/Output Control Register 12               */
  #define PORT15_IN_ADDR    	0x48028F24    	/*Port 15 Input Register                                 */
  #define PORT15_PDISC_ADDR     0x48028F60      /*Port 15 Pin Function Decision Control Register         */
  #define PORT15_PPS_ADDR    	0x48028F70    	/*Port 15 Pin Power Save Register                        */
  #define PORT15_HWSEL_ADDR     0x48028F74    	/*Port 15 Pin Hardware Select Register                   */


//port 2
  #define PORT2_OUT_ADDR    0x48028200/*  Port 2 Output Register                                 */
  #define PORT2_OMR_ADDR    0x48028204/*  Port 2 Output Modification Register                    */
  #define PORT2_IOCR0_ADDR    0x48028210/*  Port 2 Input/Output Control Register 0                 */
  #define PORT2_IOCR4_ADDR    0x48028214/*  Port 2 Input/Output Control Register 4                 */
  #define PORT2_IOCR8_ADDR    0x48028218/*  Port 2 Input/Output Control Register 8                 */
  #define PORT2_IOCR12_ADDR    0x4802821C/*  Port 2 Input/Output Control Register 12                */
  #define PORT2_IN_ADDR    0x48028224/*  Port 2 Input Register                                  */
  #define PORT2_PDR0_ADDR    0x48028240/*  Port 2 Pad Driver Mode 0 Register                      */
  #define PORT2_PDR1_ADDR    0x48028244/*  Port 2 Pad Driver Mode 1 Register                      */
  #define PORT2_PDISC_ADDR    0x48028260/*  Port 2 Pin Function Decision Control Register          */
  #define PORT2_PPS_ADDR    0x48028270/*  Port 2 Pin Power Save Register                         */
  #define PORT2_HWSEL_ADDR    0x48028274/*  Port 2 Pin Hardware Select Register                    */

//port 0
  #define PORT0_OUT_ADDR    0x48028000   /* Port 0 Output Register                                 */
  #define PORT0_OMR_ADDR    0x48028004   /* Port 0 Output Modification Register                    */
  #define PORT0_IOCR0_ADDR    0x48028010   /* Port 0 Input/Output Control Register 0                 */
  #define PORT0_IOCR4_ADDR    0x48028014   /* Port 0 Input/Output Control Register 4                 */
  #define PORT0_IOCR8_ADDR    0x48028018   /* Port 0 Input/Output Control Register 8                 */
  #define PORT0_IOCR12_ADDR    0x4802801C   /* Port 0 Input/Output Control Register 12                */
  #define PORT0_IN_ADDR    0x48028024   /* Port 0 Input Register                                  */
  #define PORT0_PDR0_ADDR    0x48028040   /* Port 0 Pad Driver Mode 0 Register                      */
  #define PORT0_PDR1_ADDR    0x48028044   /* Port 0 Pad Driver Mode 1 Register                      */
  #define PORT0_PDISC_ADDR    0x48028060   /* Port 0 Pin Function Decision Control Register          */
  #define PORT0_PPS_ADDR    0x48028070   /* Port 0 Pin Power Save Register                         */
  #define PORT0_HWSEL_ADDR    0x48028074   /* Port 0 Pin Hardware Select Register                    */


#define PORT1_OUT_ADDR     0x48028100/* Port 1 Output Register                                 */
#define PORT1_OMR_ADDR     0x48028104/* Port 1 Output Modification Register                    */
 #define PORT1_IOCR0_ADDR     0x48028110 /* Port 1 Input/Output Control Register 0                 */
 #define PORT1_IOCR4_ADDR     0x48028114/* Port 1 Input/Output Control Register 4                 */
 #define PORT1_IOCR8_ADDR     0x48028118/* Port 1 Input/Output Control Register 8                 */
 #define PORT1_IOCR12_ADDR     0x4802811C/* Port 1 Input/Output Control Register 12                */
#define PORT1_IN_ADDR     0x48028124/* Port 1 Input Register                                  */
 #define PORT1_PDR0_ADDR     0x48028140/* Port 1 Pad Driver Mode 0 Register                      */
 #define PORT1_PDR1_ADDR     0x48028144/* Port 1 Pad Driver Mode 1 Register                      */
#define PORT1_PDISC_ADDR     0x48028160/* Port 1 Pin Function Decision Control Register          */
 #define PORT1_PPS_ADDR     0x48028170/* Port 1 Pin Power Save Register                         */
 #define PORT1_HWSEL_ADDR     0x48028174/* Port 1 Pin Hardware Select Register                    */



 #define NVIC_ISER3_ADDR     0xE000E10C //Interrupt Set-enable Register 3
 #define NVIC_ICER3_ADDR     0xE000E18C //Interrupt Clear-enable Register 3
 #define NVIC_ISPR3_ADDR     0xE000E20C //Interrupt Set-pending Register 3
 #define NVIC_ICPR3_ADDR     0xE000E28C //Interrupt Clear-pending Register 3
 #define NVIC_IABR3_ADDR     0xE000E30C //Interrupt Active Bit Register 3

  #define  PPB_RESERVED [2];
   #define  PPB_ACTLR_ADDR     0xE000E008//Auxiliary Control Register                             */
  #define  PPB_RESERVED1 ;
   #define  PPB_SYST_CSR_ADDR     0xE000E010//SysTick Control and Status Register                    */
   #define  PPB_SYST_RVR_ADDR     0xE000E014//SysTick Reload Value Register                          */
   #define  PPB_SYST_CVR_ADDR     0xE000E018//SysTick Current Value Register                         */
   #define  PPB_SYST_CALIB_ADDR     0xE000E01C//SysTick Calibration Value Register r                   */
  #define  PPB_RESERVED2 [56];
   #define  PPB_NVIC_ISER0_ADDR     0xE000E100//Interrupt Set-enable Register 0                        */
   #define  PPB_NVIC_ISER1_ADDR     0xE000E104//Interrupt Set-enable Register 1                        */
   #define  PPB_NVIC_ISER2_ADDR     0xE000E108//Interrupt Set-enable Register 2                        */
   #define  PPB_NVIC_ISER3_ADDR     0xE000E10C//Interrupt Set-enable Register 3                        */
  #define  PPB_RESERVED3 [28];
   #define  PPB_NVIC_ICER0_ADDR     0xE000E180//Interrupt Clear-enable Register 0                      */
   #define  PPB_NVIC_ICER1_ADDR     0xE000E184//Interrupt Clear-enable Register 1                      */
   #define  PPB_NVIC_ICER2_ADDR     0xE000E188//Interrupt Clear-enable Register 2                      */
   #define  PPB_NVIC_ICER3_ADDR     0xE000E18C//Interrupt Clear-enable Register 3                      */
  #define  PPB_RESERVED4 [28];
   #define  PPB_NVIC_ISPR0_ADDR     0xE000E200//Interrupt Set-pending Register 0                       */
   #define  PPB_NVIC_ISPR1_ADDR     0xE000E204//Interrupt Set-pending Register 1                       */
   #define  PPB_NVIC_ISPR2_ADDR     0xE000E208//Interrupt Set-pending Register 2                       */
   #define  PPB_NVIC_ISPR3_ADDR     0xE000E20C//Interrupt Set-pending Register 3                       */
  #define  PPB_RESERVED5 [28];
   #define  PPB_NVIC_ICPR0_ADDR     0xE000E280//Interrupt Clear-pending Register 0                     */
   #define  PPB_NVIC_ICPR1_ADDR     0xE000E284//Interrupt Clear-pending Register 1                     */
   #define  PPB_NVIC_ICPR2_ADDR     0xE000E288//Interrupt Clear-pending Register 2                     */
   #define  PPB_NVIC_ICPR3_ADDR     0xE000E28C//Interrupt Clear-pending Register 3                     */
  #define  PPB_RESERVED6 [28];
   #define  PPB_NVIC_IABR0_ADDR     0xE000E300//Interrupt Active Bit Register 0                        */
   #define  PPB_NVIC_IABR1_ADDR     0xE000E304//Interrupt Active Bit Register 1                        */
   #define  PPB_NVIC_IABR2_ADDR     0xE000E308//Interrupt Active Bit Register 2                        */
   #define  PPB_NVIC_IABR3_ADDR     0xE000E30C//Interrupt Active Bit Register 3                        */
  #define  PPB_RESERVED7 [60];
   #define  PPB_NVIC_IPR0_ADDR     0xE000E400//Interrupt Priority Register 0                          */
   #define  PPB_NVIC_IPR1_ADDR     0xE000E404//Interrupt Priority Register 1                          */
   #define  PPB_NVIC_IPR2_ADDR     0xE000E408//Interrupt Priority Register 2                          */
   #define  PPB_NVIC_IPR3_ADDR     0xE000E40C//Interrupt Priority Register 3                          */
   #define  PPB_NVIC_IPR4_ADDR     0xE000E410//Interrupt Priority Register 4                          */
   #define  PPB_NVIC_IPR5_ADDR     0xE000E414//Interrupt Priority Register 5                          */
   #define  PPB_NVIC_IPR6_ADDR     0xE000E418//Interrupt Priority Register 6                          */
   #define  PPB_NVIC_IPR7_ADDR     0xE000E41C//Interrupt Priority Register 7                          */
   #define  PPB_NVIC_IPR8_ADDR     0xE000E420//Interrupt Priority Register 8                          */
   #define  PPB_NVIC_IPR9_ADDR     0xE000E424//Interrupt Priority Register 9                          */
   #define  PPB_NVIC_IPR10_ADDR     0xE000E428//Interrupt Priority Register 10                         */
   #define  PPB_NVIC_IPR11_ADDR     0xE000E42C//Interrupt Priority Register 11                         */
   #define  PPB_NVIC_IPR12_ADDR     0xE000E430//Interrupt Priority Register 12                         */
   #define  PPB_NVIC_IPR13_ADDR     0xE000E434//Interrupt Priority Register 13                         */
   #define  PPB_NVIC_IPR14_ADDR     0xE000E438//Interrupt Priority Register 14                         */
   #define  PPB_NVIC_IPR15_ADDR     0xE000E43C//Interrupt Priority Register 15                         */
   #define  PPB_NVIC_IPR16_ADDR     0xE000E440//Interrupt Priority Register 16                         */
   #define  PPB_NVIC_IPR17_ADDR     0xE000E444//Interrupt Priority Register 17                         */
   #define  PPB_NVIC_IPR18_ADDR     0xE000E448//Interrupt Priority Register 18                         */
   #define  PPB_NVIC_IPR19_ADDR     0xE000E44C//Interrupt Priority Register 19                         */
   #define  PPB_NVIC_IPR20_ADDR     0xE000E450//Interrupt Priority Register 20                         */
   #define  PPB_NVIC_IPR21_ADDR     0xE000E454//Interrupt Priority Register 21                         */
   #define  PPB_NVIC_IPR22_ADDR     0xE000E458//Interrupt Priority Register 22                         */
   #define  PPB_NVIC_IPR23_ADDR     0xE000E45C//Interrupt Priority Register 23                         */
   #define  PPB_NVIC_IPR24_ADDR     0xE000E460//Interrupt Priority Register 24                         */
   #define  PPB_NVIC_IPR25_ADDR     0xE000E464//Interrupt Priority Register 25                         */
   #define  PPB_NVIC_IPR26_ADDR     0xE000E468//Interrupt Priority Register 26                         */
   #define  PPB_NVIC_IPR27_ADDR     0xE000E46C//Interrupt Priority Register 27                         */
  #define  PPB_RESERVED8 [548];
  #define  PPB_CPUID_ADDR     0xE000ED00//CPUID Base Register                                    */
   #define  PPB_ICSR_ADDR     0xE000ED04//Interrupt Control and State Register                   */
   #define  PPB_VTOR_ADDR     0xE000ED08//Vector Table Offset Register                           */
   #define  PPB_AIRCR_ADDR     0xE000ED0C//Application Interrupt and Reset Control Register       */
   #define  PPB_SCR_ADDR     0xE000ED10//System Control Register                                */
   #define  PPB_CCR_ADDR     0xE000ED14//Configuration and Control Register                     */
   #define  PPB_SHPR1_ADDR     0xE000ED18//System Handler Priority Register 1                     */
   #define  PPB_SHPR2_ADDR     0xE000ED1C//System Handler Priority Register 2                     */
   #define  PPB_SHPR3_ADDR     0xE000ED20//System Handler Priority Register 3                     */
   #define  PPB_SHCSR_ADDR     0xE000ED24//System Handler Control and State Register              */
   #define  PPB_CFSR_ADDR     0xE000ED28//Configurable Fault Status Register                     */
   #define  PPB_HFSR_ADDR     0xE000ED2C//HardFault Status Register                              */
  #define  PPB_RESERVED9 ;
   #define  PPB_MMFAR_ADDR     0xE000ED34//MemManage Fault Address Register                       */
   #define  PPB_BFAR_ADDR     0xE000ED38//BusFault Address Register                              */
   #define  PPB_AFSR_ADDR     0xE000ED3C//Auxiliary Fault Status Register                        */
  #define  PPB_RESERVED10 [18];
   #define  PPB_CPACR_ADDR     0xE000ED88//Coprocessor Access Control Register                    */
  #define  PPB_RESERVED11 ;
  #define  PPB_MPU_TYPE_ADDR      0xE000ED90//MPU Type Register                                      */
   #define  PPB_MPU_CTRL_ADDR      0xE000ED94//MPU Control Register                                   */
   #define  PPB_MPU_RNR_ADDR     0xE000ED98//MPU Region Number Register                             */
   #define  PPB_MPU_RBAR_ADDR      0xE000ED9C//MPU Region Base Address Register                       */
   #define  PPB_MPU_RASR_ADDR      0xE000EDA0//MPU Region Attribute and Size Register                 */
   #define  PPB_MPU_RBAR_A1_ADDR     0xE000EDA4//MPU Region Base Address Register A1                    */
   #define  PPB_MPU_RASR_A1_ADDR     0xE000EDA8//MPU Region Attribute and Size Register A1              */
   #define  PPB_MPU_RBAR_A2_ADDR     0xE000EDAC//MPU Region Base Address Register A2                    */
   #define  PPB_MPU_RASR_A2_ADDR     0xE000EDB0//MPU Region Attribute and Size Register A2              */
   #define  PPB_MPU_RBAR_A3_ADDR     0xE000EDB4//MPU Region Base Address Register A3                    */
   #define  PPB_MPU_RASR_A3_ADDR     0xE000EDB8//MPU Region Attribute and Size Register A3              */
  #define  PPB_RESERVED12 [81];
  #define  PPB_STIR_ADDR     0xE000EF00//Software Trigger Interrupt Register                    */
  #define  PPB_RESERVED13 [12];
   #define  PPB_FPCCR_ADDR     0xE000EF34//Floating-point Context Control Register                */
   #define  PPB_FPCAR_ADDR     0xE000EF38//Floating-point Context Address Register                */
   #define  PPB_FPDSCR_ADDR     0xE000EF3C//Floating-point Default Status Control Register         */

/*

#define  SCU_REG_ID_ADDR             0x50004000
#define  SCU_REG_IDCHIP_ADDR         0x50004004
#define  SCU_REG_IDMANUF_ADDR        0x50004008
#define  SCU_REG_RESERVED;
#define  SCU_REG_STCON_ADDR          0x50004010
#define  SCU_REG_RESERVED1 [6];
#define  SCU_REG_GPR[2]_ADDR         0x5000402C
#define  SCU_REG_RESERVED2 [6];
#define  SCU_REG_CCUCON_ADDR         0x5000404C
#define  SCU_REG_RESERVED3 [15];
#define  SCU_REG_DTSCON_ADDR         0x5000408C
#define  SCU_REG_DTSSTAT_ADDR        0x50004090
#define  SCU_REG_RESERVED4 [2];
#define  SCU_REG_SDMMCDEL_ADDR       0x5000409C
#define  SCU_REG_GORCEN [2]_ADDR      0x500040A0
#define  SCU_REG_RESERVED5 [7];
#define  SCU_REG_MIRRSTS_ADDR        0x500040C4
#define  SCU_REG_RMACR_ADDR          0x500040C8
#define  SCU_REG_RMDATA_ADDR         0x500040CC


*/






































