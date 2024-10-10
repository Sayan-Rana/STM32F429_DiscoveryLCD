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
#include "bsp_lcd.h"

#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif


void SystemClock_Setup(void);
void LTDC_Pin_Init(void);

int main(void) {
	SystemClock_Setup();
	LTDC_Pin_Init();
	BSP_LCD_Init();
    /* Loop forever even all time */
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
	REG_SET_VAL(pRCC->CFGR, 0x00u, 0x0Fu, RCC_CFGR_HPRE_Pos);		/* AHB prescaler */

	//6. Turning on the PLL and wait for PLLCLK ready
	REG_SET_BIT(pRCC->CR, RCC_CR_PLLON_Pos);				/* Turning on PLL */
	while(!REG_READ_BIT(pRCC->CR, RCC_CR_PLLRDY_Pos));		/* Wait until main PLL clock is ready */

	//7. Switch PLLCLK as SYSCLK
	REG_SET_VAL(pRCC->CFGR, 0x02u, 0x03u, RCC_CFGR_SW_Pos);						/* Set PLL as main system clock source */
	while(!(REG_READ_VAL(pRCC->CFGR, 0x03u, RCC_CFGR_SWS_Pos) == 0x02u));		/* Wait until PLL is successfully set as the main system clock */
}



void LTDC_Pin_Init(void) {

	/* Enabling GPIO ports clock used for LTDC peripheral */
	REG_SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN_Pos);                               /* GPIOA clock enable */
	REG_SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN_Pos);                               /* GPIOB clock enable */
	REG_SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOCEN_Pos);                               /* GPIOC clock enable */
	REG_SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIODEN_Pos);                               /* GPIOD clock enable */
	REG_SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOFEN_Pos);                               /* GPIOF clock enable */
	REG_SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOGEN_Pos);                               /* GPIOG clock enable */

	for(int i = 0; i < total_ltdc_pins; i++) {
		REG_SET_VAL(ltdc_io_ports[i]->MODER, 0x02u, 0x03u, (ltdc_io_pins[i] * 2u));   /* GPIO mode register as alternate function mode */
		REG_CLR_BIT(ltdc_io_ports[i]->OTYPER, ltdc_io_pins[i]);                       /* GPIO output type register as push-pull */
		REG_SET_VAL(ltdc_io_ports[i]->OSPEEDR, 0x02u, 0x03u, (ltdc_io_pins[i] * 2u)); /* GPIO output speed as high speed */
	}

	/* Configuring ALTERNATE FUNCTION for LTDC pins */
	// Alternate function low register
	REG_SET_VAL(LCD_DATA_R3_PORT->AFR[0], 0x09u, 0x0Fu, (LCD_DATA_R3_PIN * 4u));          /* LCD_R3 */
	REG_SET_VAL(LCD_DATA_R6_PORT->AFR[0], 0x09u, 0x0Fu, (LCD_DATA_R6_PIN * 4u));          /* LCD_R6 */
	REG_SET_VAL(LCD_DATA_R7_PORT->AFR[0], 0x0Eu, 0x0Fu, (LCD_DATA_R7_PIN * 4u));          /* LCD_R7 */
	REG_SET_VAL(LCD_DATA_G2_PORT->AFR[0], 0x0Eu, 0x0Fu, (LCD_DATA_G2_PIN * 4u));          /* LCD_G2 */
	REG_SET_VAL(LCD_DATA_G6_PORT->AFR[0], 0x0Eu, 0x0Fu, (LCD_DATA_G6_PIN * 4u));          /* LCD_G6 */
	REG_SET_VAL(LCD_DATA_G7_PORT->AFR[0], 0x0Eu, 0x0Fu, (LCD_DATA_G7_PIN * 4u));          /* LCD_G7 */
	REG_SET_VAL(LCD_DATA_B2_PORT->AFR[0], 0x0Eu, 0x0Fu, (LCD_DATA_B2_PIN * 4u));          /* LCD_B2 */
	REG_SET_VAL(LCD_DATA_B5_PORT->AFR[0], 0x0Eu, 0x0Fu, (LCD_DATA_B5_PIN * 4u));          /* LCD_B5 */
	REG_SET_VAL(LCD_VSYNC_PORT->AFR[0], 0x0Eu, 0x0Fu, (LCD_VSYNC_PIN * 4u));              /* LCD_V-SYNC */
	REG_SET_VAL(LCD_HSYNC_PORT->AFR[0], 0x0Eu, 0x0Fu, (LCD_HSYNC_PIN * 4u));              /* LCD_H-SYNC */
	REG_SET_VAL(LCD_DOTCLK_PORT->AFR[0], 0x0Eu, 0x0Fu, (LCD_DOTCLK_PIN * 4u));            /* LCD_D-CLK */

	// Alternate function high register
	REG_SET_VAL(LCD_DATA_R2_PORT->AFR[1], 0x0Eu, 0x0Fu, ((LCD_DATA_R2_PIN % 8u) * 4u));   /* LCD_R2 */
	REG_SET_VAL(LCD_DATA_R4_PORT->AFR[1], 0x0Eu, 0x0Fu, ((LCD_DATA_R4_PIN % 8u) * 4u));   /* LCD_R4 */
	REG_SET_VAL(LCD_DATA_R5_PORT->AFR[1], 0x0Eu, 0x0Fu, ((LCD_DATA_R5_PIN % 8u) * 4u));   /* LCD_R5 */
	REG_SET_VAL(LCD_DATA_G3_PORT->AFR[1], 0x09u, 0x0Fu, ((LCD_DATA_G3_PIN % 8u) * 4u));   /* LCD_G3 */
	REG_SET_VAL(LCD_DATA_G4_PORT->AFR[1], 0x0Eu, 0x0Fu, ((LCD_DATA_G4_PIN % 8u) * 4u));   /* LCD_G4 */
	REG_SET_VAL(LCD_DATA_G5_PORT->AFR[1], 0x0Eu, 0x0Fu, ((LCD_DATA_G5_PIN % 8u) * 4u));   /* LCD_G5 */
	REG_SET_VAL(LCD_DATA_B3_PORT->AFR[1], 0x0Eu, 0x0Fu, ((LCD_DATA_B3_PIN % 8u) * 4u));   /* LCD_B3 */
	REG_SET_VAL(LCD_DATA_B4_PORT->AFR[1], 0x09u, 0x0Fu, ((LCD_DATA_B4_PIN % 8u) * 4u));   /* LCD_B4 */
	REG_SET_VAL(LCD_DATA_B6_PORT->AFR[1], 0x0Eu, 0x0Fu, ((LCD_DATA_B6_PIN % 8u) * 4u));   /* LCD_B6 */
	REG_SET_VAL(LCD_DATA_B7_PORT->AFR[1], 0x0Eu, 0x0Fu, ((LCD_DATA_B7_PIN % 8u) * 4u));   /* LCD_B7 */
	REG_SET_VAL(LCD_DE_PORT->AFR[1], 0x0Eu, 0x0Fu, ((LCD_DE_PIN % 8u) * 4u));             /* LCD_DE */
}









