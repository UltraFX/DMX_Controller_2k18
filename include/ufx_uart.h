#ifndef UFX_UART
#define UFX_UART

#include <stdint.h>

#define DMX_CHANNELS	512

extern void uartInit(void);
extern void uartSendByte(const char cByte);
extern void uartSendString(const char *pcBuffer, unsigned long ulCount);

uint8_t uartBusy(void);
void uartDmxSendByte(uint8_t byData);
void uartDmxSendByteNonBlocking(uint8_t byData);

extern void uartDMXRate(void);
extern void uartBreakRate(void);

#endif
