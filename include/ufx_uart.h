#ifndef UFX_UART
#define UFX_UART

#include <stdint.h>

#define DMX_CHANNELS	512

void uartInit(void);
void uartSendByte(const char cByte);
void uartSendString(const char *pcBuffer, unsigned long ulCount);

void uartDMXSend(void);

extern volatile uint8_t byaDmxData[DMX_CHANNELS];

#endif
