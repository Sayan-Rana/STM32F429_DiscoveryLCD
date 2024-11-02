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
#define BSP_LCD_PORTRAIT            0u // (Portrait mode)
#define BSP_LCD_LANDSCAPE           1u // (Landscape mode)

#define LCD_ORIENTATION             BSP_LCD_PORTRAIT
////////////////////////////////////////////////////////////////////


/* ILI9341 RGB interface HSW, HBP, HFP, VSW, VBP, VFP, ACTIVE_WIDTH & ACTIVE_HEIGHT timings */
#define BSP_LCD_HSW                 10u     /* < Horizontal synchronization > */
#define BSP_LCD_HBP                 20u     /* < Horizontal back porch > */
#define BSP_LCD_HFP                 10u     /* < Horizontal front porch > */

#define BSP_LCD_VSW                 2u      /* < Vertical synchronization > */
#define BSP_LCD_VBP                 2u      /* < Vertical back porch > */
#define BSP_LCD_VFP                 4u      /* < Vertical front porch > */

#define BSP_LCD_WIDTH               240u
#define BSP_LCD_HEIGHT              320u

#if(LCD_ORIENTATION == BSP_LCD_PORTRAIT)
#define BSP_LCD_ACTIVE_WIDTH        BSP_LCD_WIDTH
#define BSP_LCD_ACTIVE_HEIGHT       BSP_LCD_HEIGHT
#elif (LCD_ORIENTATION == BSP_LCD_LANDSCAPE)
#define BSP_LCD_ACTIVE_WIDTH        BSP_LCD_HEIGHT
#define BSP_LCD_ACTIVE_HEIGHT       (BSP_LCD_WIDTH + 80u)
#else
	#error"Select display orientation"
#endif
/////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////// LCD Pixel format //////////////////////////////////////////
#define BSP_LCD_PIXEL_FMT_L8        1u
#define BSP_LCD_PIXEL_FMT_RGB565    2u
#define BSP_LCD_PIXEL_FMT_RGB666    3u
#define BSP_LCD_PIXEL_FMT_RGB888    4u

#define BSP_LCD_PIXEL_FMT           BSP_LCD_PIXEL_FMT_RGB565
/////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////// LCD Layer width and height ///////////////////////////////
#define BSP_LCD_LAYER_HEIGHT        BSP_LCD_ACTIVE_HEIGHT
#define BSP_LCD_LAYER_WIDTH         BSP_LCD_ACTIVE_WIDTH

#define BSP_FB_HEIGHT               BSP_LCD_LAYER_HEIGHT
#define BSP_FB_WIDTH                BSP_LCD_LAYER_WIDTH
/////////////////////////////////////////////////////////////////////////////////////////////


/* Array of type GPIO_Typedef* to store all LTDC IO ports */
extern GPIO_TypeDef* ltdc_io_ports[];

/* Array of type uint8_t to store all LTDC IO pins */
extern const uint8_t ltdc_io_pins[];

/* Store total number of pins, used in LTDC peripheral */
extern const uint8_t total_ltdc_pins;


void BSP_LCD_Init(void);
uint32_t bsp_lcd_get_fb_address(void);


#endif /* BSP_LCD_H_ */
