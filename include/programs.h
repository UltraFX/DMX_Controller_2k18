#ifndef _PROGRAMS_H
#define _PROGRAMS_H

typedef enum
{
	PROG_IDLE = 0,
	PROG_MOOD
} prog_state_t;

typedef struct
{
	uint16_t wHue;
	uint8_t bySat;
	uint8_t byVal;
} hsv_t;

typedef struct
{
	uint8_t byR;
	uint8_t byG;
	uint8_t byB;
} rgb_t;

void progSet(prog_state_t eState);
prog_state_t progGet(void);

void progHandler(void);

void progCallback(void);

#endif
