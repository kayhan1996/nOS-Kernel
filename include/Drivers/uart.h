#ifndef UART_H
#define UART_H

void init_uart();
void send(char c);
char receive();
void putc(void* p, char c);

#endif  /*UART_H*/