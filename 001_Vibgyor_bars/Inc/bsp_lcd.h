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

/* Array of type GPIO_Typedef* to store all LTDC IO ports */
extern const GPIO_TypeDef* ltdc_io_port[];
/* Array of type uint8_t to store all LTDC IO pins */
extern const uint8_t ltdc_io_pins[];
/* Store total number of pins, used in LTDC peripheral */
extern const uint8_t ltdc_total_pin;

void BSP_LCD_Init(void);


#endif /* BSP_LCD_H_ */
