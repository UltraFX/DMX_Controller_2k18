#ifndef _PROGRAMS_H
#define _PROGRAMS_H

typedef enum
{
	PROG_IDLE = 0,
	PROG_MOOD,
	PROG_TRANSITION
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

extern void progSet(prog_state_t eState);
extern prog_state_t progGet(void);

extern void progHandler(void);

extern void progCallback(void);

extern void progSetColor(rgb_t sCol);
extern void progSetWhite(uint8_t byVal);

#endif
