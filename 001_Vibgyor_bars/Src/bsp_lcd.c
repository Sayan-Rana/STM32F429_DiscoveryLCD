/*
 * bsp_lcd.c
 *
 *  Created on: Aug 31, 2024
 *      Author: sayan
 */

#include "stm32f429xx.h"


void LCD_Pin_Init(void);
void LCD_SPI_Init(void);
void LCD_Reset(void);
void LCD_Config(void);


/* Defining all the LCD signals */

#ifdef STM32F429I_DISC1
	#define SPI							(SPI5)
	#define LCD_SCL_PIN					(7)
	#define LCD_SCL_PORT				(GPIOF)
	#define LCD_SDA_PIN					(9)
	#define LCD_SDA_PORT				(GPIOF)
	#define LCD_RESX_PIN				(7)
	#define LCD_RESX_PORT				(GPIOA)
	#define LCD_CSX_PIN					(2)
	#define LCD_CSX_PORT				(GPIOC)
	#define LCD_WRX_DCX_PIN				(13)
	#define LCD_WRX_DCX_PORT			(GPIOD)
#elif defined(STM32F407_DISC1)
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

}



void LCD_SPI_Init(void) {

}



void LCD_Reset(void) {

}



void LCD_Config(void) {

}

