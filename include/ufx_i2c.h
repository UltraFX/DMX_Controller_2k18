#ifndef _UFX_I2C_H
#define _UFX_I2C_H

#include <stdint.h>

void i2cInit(void);
void i2c_wait_eeprom(void);
uint8_t i2c_eeprom_write_byte(uint8_t byAddr, uint8_t byData);
uint8_t i2c_eeprom_read_byte(uint8_t byAddr, uint8_t *byData);
uint8_t i2c_eeprom_read(uint8_t byAddr, uint8_t *byData, uint8_t byLen);

#endif
