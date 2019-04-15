/*
 * systick_arch.h
 *
 *  Created on: 23.03.2017
 *      Author: Stalker1290
 */

#ifndef SYSTICK_ARCH_H_
#define SYSTICK_ARCH_H_

#ifdef __cplusplus
extern "C" {
#endif
	
#include "lwip/opt.h"

#if NO_SYS == 1

void LWIPTimer_Handler(void);

/**
 * @brief	Get the current systick time in milliSeconds
 * @return	current systick time in milliSeconds
 * @note	Returns the current systick time in milliSeconds. This function is only
 * used in standalone systems.
 */
uint32_t LWIPTimer_GetMS(void);

#endif

/**
 * @brief	Delay for the specified number of milliSeconds
 * @param	ms	: Time in milliSeconds to delay
 * @return	Nothing
 * @note	For standalone systems. This function will block for the specified
 * number of milliSconds. For RTOS based systems, this function will delay
 * the task by the specified number of milliSeconds.
 */
void LWIPTimer_msDelay(uint32_t ms);

u32_t sys_now(void);

#ifdef __cplusplus
}
#endif

#endif /* SYSTICK_ARCH_H_ */
