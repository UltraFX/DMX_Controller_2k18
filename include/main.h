#ifndef MAIN_H
#define MAIN_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "inc/lm3s8938.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "driverlib/adc.h"
#include "driverlib/gpio.h"
#include "driverlib/ssi.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"
#include "driverlib/pwm.h"

#include "ufx_adc.h"
#include "ufx_uart.h"
#include "ufx_timer.h"
#include "ufx_spi.h"
#include "ufx_gpio.h"
#include "ufx_pwm.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "user_interface.h"
#include "gui.h"
#include "menu.h"
#include "programs.h"

#define useFreeRTOS	1

#endif
