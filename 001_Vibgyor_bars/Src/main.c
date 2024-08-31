/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Auto-generated by STM32CubeIDE
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#include <stdint.h>
#include "stm32f429xx.h"
#include "reg_util.h"

#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif

void SystemClock_Setup(void);

int main(void) {
	SystemClock_Setup();
    /* Loop forever */
	for(;;);
}


void SystemClock_Setup(void) {
	RCC_TypeDef* pRCC = RCC;
	FLASH_TypeDef* pFlash = FLASH;
	PWR_TypeDef* pPWR = PWR;

	//1. Program flash wait state
	REG_SET_VAL(pFlash->ACR, 0x5u, 0xFu, FLASH_ACR_LATENCY_Pos);

	//2. Over drive settings
	REG_SET_BIT(pRCC->APB1ENR, RCC_APB1ENR_PWREN_Pos);		/* Enable clock for PWR peripheral */
	REG_SET_VAL(pPWR->CR, 0x3u, 0x3u, PWR_CR_VOS_Pos);		/* VOS = 0b11, Power scale 1 */
	REG_SET_BIT(pPWR->CR, PWR_CR_ODEN_Pos);					/* Activate over drive mode */
	while(!REG_READ_BIT(pPWR->CSR, PWR_CSR_ODRDY_Pos));		/* Wait for over drive ready */
	REG_SET_BIT(pPWR->CR, PWR_CR_ODSWEN_Pos);				/* Over drive switch enable */

	/*
	 * Since our PLL clock source is HSI = 16 MHz and our desired SYSCLK frequency is 180 MHz
	 * which is generated by Main PLL. First we divide HSI by 8(PLL M divider). After division
	 * the frequency is 2 MHz. Then we multiply it with 180(PLL N multiplier). The frequency
	 * become 360 MHz. After that we divide it with 2(PLL P divider). So the PLL output clock
	 * frequency is 180 MHz.
	 */
	//3. Setting up main PLL
	REG_SET_VAL(pRCC->PLLCFGR, 0x08u, 0x3Fu, RCC_PLLCFGR_PLLM_Pos);		/* PLL M */
	REG_SET_VAL(pRCC->PLLCFGR, 180u, 0x1FFu, RCC_PLLCFGR_PLLN_Pos);		/* PLL N */
	REG_SET_VAL(pRCC->PLLCFGR, 0x00u, 0x3u, RCC_PLLCFGR_PLLP_Pos);		/* PLL P */

	/*/////////////////// This step is only required if we are using RGB interface //////////////////*/
	//4. Setting up the LCD_CLK using PLLSAI block
	REG_SET_VAL(pRCC->PLLSAICFGR, 50u, 0x1FFu, RCC_PLLSAICFGR_PLLSAIN_Pos);		/* PLLSAI N */
	REG_SET_VAL(pRCC->PLLSAICFGR, 0x02u, 0x07u, RCC_PLLSAICFGR_PLLSAIR_Pos);	/* PLLSAI R */
	/* LCD CLK 6.25MHz */
	REG_SET_VAL(pRCC->DCKCFGR, 0x02u, 0x03u, RCC_DCKCFGR_PLLSAIDIVR_Pos);		/* DIV */
	/*///////////////////////////////////////////////////////////////////////////////////////////////*/

	//5. Setting up the AHB and APBx clocks
	REG_SET_VAL(pRCC->CFGR, 0x04u, 0x07u, RCC_CFGR_PPRE2_Pos);		/* APB2 prescaler */
	REG_SET_VAL(pRCC->CFGR, 0x05u, 0x07u, RCC_CFGR_PPRE1_Pos);		/* APB1 prescaler */
	REG_SET_VAL(pRCC->CFGR, 0x00u, 0x0Fu, RCC_CFGR_HPRE_Pos);		/* AHB prescaller */

	//6. Turning on the PLL and wait for PLLCLK ready
	REG_SET_BIT(pRCC->CR, RCC_CR_PLLON_Pos);				/* Turning on PLL */
	while(!REG_READ_BIT(pRCC->CR, RCC_CR_PLLRDY_Pos));		/* Wait until main PLL clock is ready */

	//7. Switch PLLCLK as SYSCLK
	REG_SET_VAL(pRCC->CFGR, 0x02u, 0x03u, RCC_CFGR_SW_Pos);						/* Set PLL as main system clock source */
	while(!(REG_READ_VAL(pRCC->CFGR, 0x03u, RCC_CFGR_SWS_Pos) == 0x02u));		/* Wait until PLL is successfully set as the main system clock */
}













