#ifndef _APP_CONFIG_H
#define _APP_CONFIG_H

/** @brief enable or disable DEBUG mode (displaying debug messages via UART) */
#define DEBUG_EN

/** @brief software revision (major - minor) */
#define VERSION			0x02

/** @brief EEPROM address areas */
#define VERSION_AREA	0
#define DEVICE_AREA		1	/* start of device addresses in EEPROM */
#define PROG_SPEED_AREA	6
#define CHG_SPEED_AREA	7
#define USER_AREA       8
#define SCENES_AREA	    28

/** @brief device start addresses */
#define DEVICE0_ADDR	1		/**< device start-address for the first lamp */
#define DEVICE1_ADDR	11		/**< device start-address for the second lamp */
#define DEVICE2_ADDR	21		/**< device start-address for the third lamp */
#define DEVICE3_ADDR	31		/**< device start-address for the fourth lamp */
#define	DEVICE4_ADDR	41		/**< device start-address for the fifth lamp */

/** @brief timeouts */
#define TIME_LONGPRESS		2000U 	/**< timeout for long keypress detection */
#define TIME_MOODLIGHT  	100U	/**< timeout for moodlight-steps */
#define TIME_TRANSITION 	2U		/**< timeout for color transition-steps */
#define TIME_RANDOM_WAIT	10000U	/**< waiting time, until next transition comes */

/** @brief general settings */
#define COLOR_SHIFT		30U // 60U

#endif
