#ifndef UFX_SPI
#define UFX_SPI

void spiInit(uint32_t dwRate);
void spiSetLength(uint8_t byLength);
void spiSend(uint16_t wData);


#endif
