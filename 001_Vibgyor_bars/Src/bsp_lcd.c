/*
 * bsp_lcd.c
 *
 *  Created on: Aug 31, 2024
 *      Author: sayan
 */

#include "stm32f429xx.h"
#include "reg_util.h"


#define GPIO_PIN_0			0U
#define GPIO_PIN_1			1U
#define GPIO_PIN_2			2U
#define GPIO_PIN_3			3U
#define GPIO_PIN_4			4U
#define GPIO_PIN_5			5U
#define GPIO_PIN_6			6U
#define GPIO_PIN_7			7U
#define GPIO_PIN_8			8U
#define GPIO_PIN_9			9U
#define GPIO_PIN_10			10U
#define GPIO_PIN_11			11U
#define GPIO_PIN_12			12U
#define GPIO_PIN_13			13U
#define GPIO_PIN_14			14U
#define GPIO_PIN_15			15U


/* Static helper functions */
static void LCD_Pin_Init(void);
static void LCD_SPI_Init(void);
static void LCD_Reset(void);
static void LCD_Config(void);


/* Defining all the LCD signals */

#ifdef STM32F429I_DISC1
	#define SPI							(SPI5)
	#define LCD_SCL_PIN					(GPIO_PIN_7)
	#define LCD_SCL_PORT				(GPIOF)
	#define LCD_SDA_PIN					(GPIO_PIN_9)
	#define LCD_SDA_PORT				(GPIOF)
	#define LCD_RESX_PIN				(GPIO_PIN_7)
	#define LCD_RESX_PORT				(GPIOA)
	#define LCD_CSX_PIN					(GPIO_PIN_2)
	#define LCD_CSX_PORT				(GPIOC)
	#define LCD_WRX_DCX_PIN				(GPIO_PIN_13)
	#define LCD_WRX_DCX_PORT			(GPIOD)
#elif defined(STM32F407_DISC1)
	#define SPI							(SPI7)
	#define LCD_SCL_PIN
	#define LCD_SCL_PORT
	#define LCD_SDA_PIN
	#define LCD_SDA_PORT
	#define LCD_RESX_PIN
	#define LCD_RESX_PORT
	#define LCD_CSX_PIN
	#define LCD_CSX_PORT
	#define LCD_WRX_DCX_PIN
	#define LCD_WRX_DCX_PORT
#else
	#error "Supported device is not selected"
#endif



void BSP_LCD_Init(void) {
	LCD_Pin_Init();
	LCD_SPI_Init();
	LCD_Reset();
	LCD_Config();
}



void LCD_Pin_Init(void) {

	RCC_TypeDef* pRCC    = RCC;
	GPIO_TypeDef* pGPIOA = GPIOA;
	GPIO_TypeDef* pGPIOC = GPIOC;
	GPIO_TypeDef* pGPIOD = GPIOD;
	GPIO_TypeDef* pGPIOF = GPIOF;

	/* Turning on clock for GPIOA, GPIOC, GPIOD peripherals */
	REG_SET_BIT(pRCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN_Pos);					// GPIOA peripheral enable
	REG_SET_BIT(pRCC->AHB1ENR, RCC_AHB1ENR_GPIOCEN_Pos);					// GPIOC peripheral enable
	REG_SET_BIT(pRCC->AHB1ENR, RCC_AHB1ENR_GPIODEN_Pos);					// GPIOD peripheral enable
	REG_SET_BIT(pRCC->AHB1ENR, RCC_AHB1ENR_GPIOFEN_Pos);					// GPIOF peripheral enable

	/* LCD RESX pin */
	REG_SET_VAL(pGPIOA->MODER, 0x01u, 0x03u, GPIO_MODER_MODER7_Pos);		// Output mode
	REG_CLR_BIT(pGPIOA->OTYPER, GPIO_OTYPER_OT7_Pos);						// Output type output push pull
	REG_SET_VAL(pGPIOA->OSPEEDR, 0x02u, 0x03u, GPIO_OSPEEDR_OSPEED7_Pos);	// Speed as high speed
	REG_CLR_VAL(pGPIOA->PUPDR, 0x03u, GPIO_PUPDR_PUPD7_Pos);				// No pull up pull down

	/* LCD CSX pin */
	REG_SET_VAL(pGPIOC->MODER, 0x01u, 0x03u, GPIO_MODER_MODER2_Pos);		// Output mode
	REG_CLR_BIT(pGPIOC->OTYPER, GPIO_OTYPER_OT2_Pos);						// Output type output push pull
	REG_SET_VAL(pGPIOC->OSPEEDR, 0x02u, 0x03u, GPIO_OSPEEDR_OSPEED2_Pos);	// Speed as high speed
	REG_CLR_VAL(pGPIOC->PUPDR, 0x03u, GPIO_PUPDR_PUPD2_Pos);				// No pull up pull down

	/* LCD WRX_DCX pin */
	REG_SET_VAL(pGPIOD->MODER, 0x01u, 0x03u, GPIO_MODER_MODER13_Pos);		// Output mode
	REG_CLR_BIT(pGPIOD->OTYPER, GPIO_OTYPER_OT13_Pos);						// Output type output push pull
	REG_SET_VAL(pGPIOD->OSPEEDR, 0x02u, 0x03u, GPIO_OSPEEDR_OSPEED13_Pos);	// Speed as high speed
	REG_CLR_VAL(pGPIOD->PUPDR, 0x03u, GPIO_PUPDR_PUPD13_Pos);				// No pull up pull down

	/* SPI(5) SCK pin PF7 */
	REG_SET_VAL(pGPIOF->MODER, 0x02u, 0x03u, GPIO_MODER_MODER7_Pos);		// Output mode
	REG_CLR_BIT(pGPIOF->OTYPER, GPIO_OTYPER_OT7_Pos);						// Output type output push pull
	REG_SET_VAL(pGPIOF->OSPEEDR, 0x02u, 0x03u, GPIO_OSPEEDR_OSPEED7_Pos);	// Speed as high speed
	REG_SET_VAL(pGPIOF->AFR[0], 0x05u, 0x0Fu, GPIO_AFRL_AFSEL7_Pos);		// Alternate function 5 (SPI5 SCK)

	/* SPI(5) SDA pin PF9 */
	REG_SET_VAL(pGPIOF->MODER, 0x02u, 0x03u, GPIO_MODER_MODER9_Pos);		// Output mode
	REG_CLR_BIT(pGPIOF->OTYPER, GPIO_OTYPER_OT9_Pos);						// Output type output push pull
	REG_SET_VAL(pGPIOF->OSPEEDR, 0x02u, 0x03u, GPIO_OSPEEDR_OSPEED9_Pos);	// Speed as high speed
	REG_SET_VAL(pGPIOF->AFR[1], 0x05u, 0x0Fu, GPIO_AFRH_AFSEL9_Pos);		// Alternate function 5 (SPI5 DATA)
}



void LCD_SPI_Init(void) {

}



void LCD_Reset(void) {

}



void LCD_Config(void) {

}

