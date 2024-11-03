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



#define RGB888(r,g,b)  (((r) << 16) | ((g) << 8) | (b))

#define VIOLET   	RGB888(148,0,211)
#define INDIGO   	RGB888(75,0,130)
#define BLUE   		RGB888(0,0,255)
#define GREEN   	RGB888(0,255,0)
#define YELLOW   	RGB888(255,255,0)
#define ORANGE   	RGB888(255,127,0)
#define RED   		RGB888(255,0,0)
#define WHITE   	RGB888(255,255,255)
#define BLACK		RGB888(0,0,0)


void SystemClock_Setup(void);
void LTDC_Pin_Init(void);
void LTDC_Init(void);
void LTDC_Layer_Init(LTDC_Layer_TypeDef *pLayer);

int main(void) {
	SystemClock_Setup();
	BSP_LCD_Init();
	LTDC_Pin_Init();
	LTDC_Init();
	bsp_lcd_set_fb_background_color(ORANGE);
	LTDC_Layer_Init(LTDC_Layer1);
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
	REG_SET_BIT(pRCC->CR,RCC_CR_PLLSAION_Pos);
	while(!REG_READ_BIT(pRCC->CR,RCC_CR_PLLSAIRDY_Pos));
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

	/* Enable the peripheral clock of GPIO ports involved in LTDC interface */
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



void LTDC_Init(void) {
	LTDC_TypeDef* pLTDC = LTDC;

	/* Enable LTDC peripheral clock */
	REG_SET_BIT(RCC->APB2ENR, RCC_APB2ENR_LTDCEN_Pos);

	/* Configure horizontal synchronization timing */
	// Horizontal sync width
	REG_SET_VAL(pLTDC->SSCR, (BSP_LCD_HSW - 1), 0xFFFu, LTDC_SSCR_HSW_Pos);
	// Accumulated horizontal back porch
	REG_SET_VAL(pLTDC->BPCR, (BSP_LCD_HSW + BSP_LCD_HBP - 1), 0xFFFu, LTDC_BPCR_AHBP_Pos);
	// Accumulated active width
	REG_SET_VAL(pLTDC->AWCR, (BSP_LCD_HSW + BSP_LCD_HBP + BSP_LCD_ACTIVE_WIDTH - 1), 0xFFFu, LTDC_AWCR_AAW_Pos);
	// Accumulated total width
	REG_SET_VAL(pLTDC->TWCR, (BSP_LCD_HSW + BSP_LCD_HBP + BSP_LCD_ACTIVE_WIDTH + BSP_LCD_HFP - 1), 0xFFFu, LTDC_TWCR_TOTALW_Pos);

	/* Configure vertical synchronization timing */
	// Vertical sync width
	REG_SET_VAL(pLTDC->SSCR, (BSP_LCD_VSW - 1), 0x7FFu, LTDC_SSCR_VSH_Pos);
	// Accumulated vertical back porch
	REG_SET_VAL(pLTDC->BPCR, (BSP_LCD_VSW + BSP_LCD_VBP - 1), 0x7FFu, LTDC_BPCR_AVBP_Pos);
	// Accumulated active height
	REG_SET_VAL(pLTDC->AWCR, (BSP_LCD_VSW + BSP_LCD_VBP + BSP_LCD_HEIGHT - 1), 0x7FFu, LTDC_AWCR_AAH_Pos);
	// Accumulated total height
	REG_SET_VAL(pLTDC->TWCR, (BSP_LCD_VSW + BSP_LCD_VBP + BSP_LCD_HEIGHT + BSP_LCD_VFP - 1), 0x7FFu, LTDC_TWCR_TOTALH_Pos);


	/* Configure the background color(RED) */
	REG_SET_VAL(pLTDC->BCCR, 0xFF0000u, 0xFFFFFFu, LTDC_BCCR_BCBLUE_Pos);

	/* Configure default polarity for HSYNC, VSYNC, DOT_CLK, DE */
	// We will use default polarity

	/* Enable the LTDC peripheral */
	REG_SET_BIT(pLTDC->GCR, LTDC_GCR_LTDCEN_Pos);
}



void LTDC_Layer_Init(LTDC_Layer_TypeDef *pLayer) {

	uint32_t temp1 = 0, temp2 = 0, temp3 = 0;
	LTDC_TypeDef *pLTDC = LTDC;

	//1. Configure the pixel format of the layer's frame buffer
	REG_SET_VAL(pLayer->PFCR, 0x02u, 0x07u, LTDC_LxPFCR_PF_Pos);

	//2. Configure the constant alpha and blending factor
	REG_SET_VAL(pLayer->CACR, 0xFFu, 0xFFu, LTDC_LxCACR_CONSTA_Pos);
	REG_SET_VAL(temp1, 0x04u, 0x07u, LTDC_LxBFCR_BF1_Pos);
	REG_SET_VAL(temp1, 0x05u, 0x07, LTDC_LxBFCR_BF2_Pos);
	REG_WRITE(pLayer->BFCR, temp1);

	//3. Configure the layer position (Windowing)

	                     /* WHSTART */
	temp1 = REG_READ_VAL(pLTDC->BPCR, 0xFFFu, LTDC_BPCR_AHBP_Pos);     // Read AHBP
	temp1 = temp1 + 1 + 0;                                             // Calculate WHSTART
	REG_SET_VAL(temp2, temp1, 0XFFFu, LTDC_LxWHPCR_WHSTPOS_Pos);       // Preparing temp2

	                     /* WHSTOP */
	temp1 = temp1 + BSP_LCD_LAYER_WIDTH;                               // Calculating WHSTOP
	temp3 = REG_READ_VAL(pLTDC->AWCR, 0xFFFu, LTDC_AWCR_AAW_Pos);      // Reading AAW from LTDC_AWCR register
	temp1 = (temp1 > temp3) ? temp3 : temp1;                           // Check if WHSTOP is grated than AAW or not
	REG_SET_VAL(temp2, temp1, 0xFFFu, LTDC_LxWHPCR_WHSPPOS_Pos);       // Preparing temp2

	/* Writing the layer horizontal windowing register */
	REG_WRITE(pLayer->WHPCR, temp2);

	                     /* WVSTART */
	temp1 = 0, temp2 = 0, temp3 = 0;
	temp1 = REG_READ_VAL(pLTDC->BPCR, 0x7FFu, LTDC_BPCR_AVBP_Pos);     // Read AVBP
	temp1 = temp1 + 1 + 0;                                             // Calculate WVSTART
	REG_SET_VAL(temp2, temp1, 0x7FF, LTDC_LxWVPCR_WVSTPOS_Pos);        // Preparing temp2

	                     /* WVSTOP */
	temp1 = temp1 + BSP_LCD_LAYER_HEIGHT;                              // Calculating WVSTOP
	temp3 = REG_READ_VAL(pLTDC->AWCR, 0x7FFu, LTDC_AWCR_AAH_Pos);      // Reading AAH from LTDC_AWCR
	temp1 = (temp1 > temp3) ? temp3 : temp1;                           // Check if WVSTOP is grater than  AAH or not
	REG_SET_VAL(temp2, temp1, 0x7FFu, LTDC_LxWVPCR_WVSPPOS_Pos);       // Preparing temp2

	/* Writing the layer vertical windowing register */
	REG_WRITE(pLayer->WVPCR, temp2);

	//4. Configure the frame buffer address
	REG_WRITE(pLayer->CFBAR, bsp_lcd_get_fb_address());

	//5. Configure the default color of the layer (Optional)

	//6. Configure the pitch, line length and line number of the frame buffer
	temp1 = 0, temp2 = 0, temp3 = 0;
	/* Pitch and line length configuration */
	temp1 = (BSP_LCD_LAYER_WIDTH * 2u);                                // Calculating Pitch
	REG_SET_VAL(temp3, temp1, 0x1FFFu, LTDC_LxCFBLR_CFBP_Pos);         // Preparing temp3
	temp2 = temp1 + 3;                                                 // Line length
	REG_SET_VAL(temp3, temp2, 0x1FFFu, LTDC_LxCFBLR_CFBLL_Pos);        // Preparing temp3

	/* Writing to LxCFBLR register */
	REG_WRITE(pLayer->CFBLR, temp3);

	/* Line number configuration */
	REG_SET_VAL(pLayer->CFBLNR, BSP_LCD_LAYER_HEIGHT, 0x7FFu, LTDC_LxCFBLNR_CFBLNBR_Pos);

	//7. Activate immediate reload
	REG_SET_BIT(LTDC->SRCR, LTDC_SRCR_IMR_Pos);

	//8. Enable the layer
	REG_SET_BIT(pLayer->CR, LTDC_LxCR_LEN_Pos);
}

