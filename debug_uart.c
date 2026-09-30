/*
 * Copyright (C) 2015 Infineon Technologies AG. All rights reserved.
 *
 * Infineon Technologies AG (Infineon) is supplying this software for use with
 * Infineon's microcontrollers.
 * This file can be freely distributed within development tools that are
 * supporting such microcontrollers.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS". NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * INFINEON SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 * OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 */

#include "debug_uart.h"
#include "xmc_uart.h"
#include "xmc_gpio.h"
#include "xmc_usic.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

int uart_console_init(void)
{
    XMC_GPIO_CONFIG_t rx_config = {
        .mode             = XMC_GPIO_MODE_INPUT_TRISTATE,
        .output_level     = XMC_GPIO_OUTPUT_LEVEL_HIGH,
        .output_strength  = XMC_GPIO_OUTPUT_STRENGTH_STRONG_SOFT_EDGE
    };

    XMC_GPIO_CONFIG_t tx_config = {
        .mode             = XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT2,
        .output_level     = XMC_GPIO_OUTPUT_LEVEL_HIGH,
        .output_strength  = XMC_GPIO_OUTPUT_STRENGTH_STRONG_SOFT_EDGE
    };

    XMC_UART_CH_CONFIG_t uart_config = {
        .baudrate        = 19200,   /* Ensure Python connects at 19200 (or set to 115200) */
        .data_bits       = 8,
        .frame_length    = 8,
        .stop_bits       = 1,
        .oversampling    = 16,
        .parity_mode     = XMC_USIC_CH_PARITY_MODE_NONE
    };

    /* Initialize and configure UART0 on channel 0 */
    XMC_UART_CH_Init(XMC_UART0_CH0, &uart_config);

    /* Configure RX pin (P1.4) */
    XMC_GPIO_Init(P1_4, &rx_config);

    /* Configure TX pin (P1.5) */
    XMC_GPIO_Init(P1_5, &tx_config);

    /* Set input source path */
    XMC_USIC_CH_SetInputSource(XMC_UART0_CH0, XMC_USIC_CH_INPUT_DX0, 1U);

    /* Configure transmit FIFO */
    XMC_USIC_CH_TXFIFO_Configure(XMC_UART0_CH0, 16U, XMC_USIC_CH_FIFO_SIZE_16WORDS, 1U);

    /* Configure receive FIFO */
    XMC_USIC_CH_RXFIFO_Configure(XMC_UART0_CH0, 0U, XMC_USIC_CH_FIFO_SIZE_16WORDS, 15U);

    /* Start UART */
    XMC_UART_CH_Start(XMC_UART0_CH0);

    return 1;
}

/* Renamed from putchar(): older newlib (GCC 4.9 in DAVE) defines putchar() as a macro in
 * <stdio.h>, so defining a function with that name gives "expected identifier or '(' before '--' token". */
int uart_putchar(int ch)
{
    XMC_UART_CH_Transmit(XMC_UART0_CH0, (uint16_t)ch);

    /* Wait for transmit buffer interrupt flag */
    while ((XMC_USIC_CH_TXFIFO_GetEvent(XMC_UART0_CH0) & XMC_USIC_CH_TXFIFO_EVENT_STANDARD) == 0U);
    XMC_USIC_CH_TXFIFO_ClearEvent(XMC_UART0_CH0, XMC_USIC_CH_TXFIFO_EVENT_STANDARD);

    return ch;
}

int _write(int file, unsigned char *buf, int nbytes)
{
    (void)file;
    for (int i = 0; i < nbytes; i++)
    {
        uart_putchar((int)buf[i]);
    }
    return nbytes;
}

/* Returns true if data is ready in the RX FIFO or channel */
bool uart_has_data(void)
{
    /* Check if RX FIFO is not empty */
    return !XMC_USIC_CH_RXFIFO_IsEmpty(XMC_UART0_CH0);
}

/* Reads 1 byte from the UART RX FIFO */
char uart_read_char(void)
{
    return (char)XMC_UART_CH_GetReceivedData(XMC_UART0_CH0);
}
