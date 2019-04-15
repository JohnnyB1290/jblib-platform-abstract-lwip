/*
 * systick_arch.c
 *
 *  Created on: 23.03.2017
 *      Author: Stalker1290
 */

#include "lwip/opt.h"

#if NO_SYS == 1

#include "chip.h"
#include "arch/systick_arch.h"

/* Saved total time in mS since timer was enabled */
static volatile u32_t systick_timems;


void LWIPTimer_Handler(void)
{
	/* Increment tick count */
	systick_timems++;
}

/* Get the current systick time in milliSeconds */
uint32_t SysTick_GetMS(void)
{
	return systick_timems;
}

/* Delay for the specified number of milliSeconds */
void LWIPTimer_msDelay(uint32_t ms)
{
	uint32_t to = ms + systick_timems;

	while (to > systick_timems) {}
}

/**
 * @brief	LWIP standalone mode time support
 * @return	Returns the current time in mS
 * @note	Returns the current time in mS. This is needed for the LWIP timers
 */
u32_t sys_now(void)
{
	return (u32_t) SysTick_GetMS();
}

/**
 * @}
 */

#endif /* NO_SYS == 1 */
