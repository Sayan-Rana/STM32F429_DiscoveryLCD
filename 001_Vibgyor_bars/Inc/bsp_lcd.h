/*
 * bsp_lcd.h
 *
 *  Created on: Aug 31, 2024
 *      Author: sayan
 */

#ifndef BSP_LCD_H_
#define BSP_LCD_H_

#include "stm32f429xx.h"
#include "reg_util.h"
#include "board.h"



///////////////////////// LCD orientation //////////////////////////
//// BSP_LCD_PORTRAIT           1 (Portrait mode)
//// BSP_LCD_PORTRAIT           0 (Landscape mode)
#define BSP_LCD_PORTRAIT            1u
////////////////////////////////////////////////////////////////////


/* ILI9341 RGB interface HSW, HBP, HFP, VSW, VBP, VFP, ACTIVE_WIDTH & ACTIVE_HIGHT timings */
#define BSP_LCD_HSW                 10u     /* < Horizontal synchronization > */
#define BSP_LCD_HBP                 20u     /* < Horizontal back porch > */
#define BSP_LCD_HFP                 10u     /* < Horizontal front porch > */

#define BSP_LCD_VSW                 2u      /* < Vertical synchronization > */
#define BSP_LCD_VBP                 2u      /* < Vertical back porch > */
#define BSP_LCD_VFP                 4u      /* < Vertical front porch > */

#if(BSP_LCD_PORTRAIT)
#define BSP_LCD_ACTIVE_WIDTH        240u
#define BSP_LCD_ACTIVE_HIGHT        320u
#else
#define BSP_LCD_ACTIVE_WIDTH        320u
#define BSP_LCD_ACTIVE_HIGHT        240u
#endif
/////////////////////////////////////////////////////////////////////////////////////////////


/* Array of type GPIO_Typedef* to store all LTDC IO ports */
extern GPIO_TypeDef* ltdc_io_ports[];

/* Array of type uint8_t to store all LTDC IO pins */
extern const uint8_t ltdc_io_pins[];

/* Store total number of pins, used in LTDC peripheral */
extern const uint8_t total_ltdc_pins;


void BSP_LCD_Init(void);


#endif /* BSP_LCD_H_ */
