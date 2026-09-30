#ifndef DEBUG_UART_H_
#define DEBUG_UART_H_

#include <stdbool.h>
#include <stdint.h>

int uart_console_init(void);
int uart_putchar(int ch);
bool uart_has_data(void);
char uart_read_char(void);

#endif /* DEBUG_UART_H_ */
