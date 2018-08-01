#ifndef _PROGRAMS_H
#define _PROGRAMS_H

/**  program states */
typedef enum
{
	PROG_IDLE = 0,
	PROG_BLACKOUT,
	PROG_MOOD,
	PROG_MOOD_DELAY,
	PROG_TRANSITION,
	PROG_RAND_ALL,
	PROG_RAND_EACH,
	PROG_MANUAL
} prog_state_t;

/**  HSV datatype */
typedef struct
{
	uint16_t wHue;
	uint8_t bySat;
	uint8_t byVal;
} hsv_t;

/**  RGB data type */
typedef struct
{
	uint8_t byR;
	uint8_t byG;
	uint8_t byB;
} rgb_t;

/**  
*	@brief set current program state 
*
*	@param[in]	eState	program state
*/
extern void progSet(prog_state_t eState);

/**  
*	@brief get current program state
*
*	@preturn			current program state
*/
extern prog_state_t progGet(void);

/**  @brief cyclic program handler, should be called in idle- or task loop */
extern void progHandler(void);

/**  @brief program timer interrupt callback, should be called in 1 ms timer interrupt */
extern void progCallback(void);

/**  
*	@brief set desired color for all 5 lamps 
*
*	@param[in]	sCol	desired color
*/
extern void progSetColor(rgb_t sCol);

/**  
*	@brief set desired brightness for white channel
*
*	@param[in]	byVal	brightness value (0 - 255)
*/
extern void progSetWhite(uint8_t byVal);

/**  
*	@brief activate scene
*
*	@param[in]	byScene	 scene to set (0 - 4)
*/
extern uint8_t progSetScene(uint8_t byScene);

#endif
