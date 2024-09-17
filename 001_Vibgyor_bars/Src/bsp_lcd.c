/*
 * bsp_lcd.c
 *
 *  Created on: Aug 31, 2024
 *      Author: sayan
 */

#include "stm32f429xx.h"
#include "reg_util.h"
#include "ili9341_reg.h"



#define MADCTL_MY 0x80  ///< Bottom to top
#define MADCTL_MX 0x40  ///< Right to left
#define MADCTL_MV 0x20  ///< Reverse Mode
#define MADCTL_ML 0x10  ///< LCD refresh Bottom to top
#define MADCTL_RGB 0x00 ///< Red-Green-Blue pixel order
#define MADCTL_BGR 0x08 ///< Blue-Green-Red pixel order
#define MADCTL_MH 0x04  ///< LCD refresh right to left


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


/* Defining all the LCD signals */

#ifdef STM32F429I_DISC1
	#define SPI                (SPI5)
	#define LCD_SCL_PIN        (GPIO_PIN_7)
	#define LCD_SCL_PORT       (GPIOF)
	#define LCD_SDA_PIN        (GPIO_PIN_9)
	#define LCD_SDA_PORT       (GPIOF)
	#define LCD_RESX_PIN       (GPIO_PIN_7)
	#define LCD_RESX_PORT      (GPIOA)
	#define LCD_CSX_PIN        (GPIO_PIN_2)
	#define LCD_CSX_PORT       (GPIOC)
	#define LCD_WRX_DCX_PIN    (GPIO_PIN_13)
	#define LCD_WRX_DCX_PORT   (GPIOD)
#elif defined(STM32F407_DISC1)
	#define SPI                (SPI7)
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


/* LCD RESX pin high and low macro */
#define LCD_RESX_HIGH()                 (REG_SET_BIT(LCD_RESX_PORT->ODR, GPIO_ODR_OD7_Pos))
#define LCD_RESX_LOW()                  (REG_CLR_BIT(LCD_RESX_PORT->ODR, GPIO_ODR_OD7_Pos))


/* LCD CSX pin high and low macro */
#define LCD_CSX_HIGH()                  (REG_SET_BIT(LCD_CSX_PORT->ODR, GPIO_ODR_OD2_Pos))
#define LCD_CSX_LOW()                   (REG_CLR_BIT(LCD_CSX_PORT->ODR, GPIO_ODR_OD2_Pos))


/* LCD DCX pin high and low macro */
#define LCD_DCX_HIGH()                  (REG_SET_BIT(LCD_WRX_DCX_PORT->ODR, GPIO_ODR_OD13_Pos))
#define LCD_DCX_LOW()                   (REG_CLR_BIT(LCD_WRX_DCX_PORT->ODR, GPIO_ODR_OD13_Pos))


/* Static helper functions */
static void LCD_Pin_Init(void);
static void LCD_SPI_Init(void);
static void LCD_Reset(void);
static void LCD_Config(void);
static void LCD_SPI_Enable(void);
static void LCD_Write_Cmd(uint8_t cmd);
static void LCD_Write_Data(uint8_t *pData, uint32_t len);
void delay_50ms(void);


void BSP_LCD_Init(void) {
	LCD_Pin_Init();
	LCD_SPI_Init();
	LCD_SPI_Enable();
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
	REG_SET_VAL(pGPIOA->MODER, 0x01u, 0x03u, GPIO_MODER_MODER7_Pos);        // Output mode
	REG_CLR_BIT(pGPIOA->OTYPER, GPIO_OTYPER_OT7_Pos);                       // Output type output push pull
	REG_SET_VAL(pGPIOA->OSPEEDR, 0x02u, 0x03u, GPIO_OSPEEDR_OSPEED7_Pos);   // Speed as high speed
	REG_CLR_VAL(pGPIOA->PUPDR, 0x03u, GPIO_PUPDR_PUPD7_Pos);                // No pull up pull down

	/* LCD CSX pin */
	REG_SET_VAL(pGPIOC->MODER, 0x01u, 0x03u, GPIO_MODER_MODER2_Pos);        // Output mode
	REG_CLR_BIT(pGPIOC->OTYPER, GPIO_OTYPER_OT2_Pos);                       // Output type output push pull
	REG_SET_VAL(pGPIOC->OSPEEDR, 0x02u, 0x03u, GPIO_OSPEEDR_OSPEED2_Pos);   // Speed as high speed
	REG_CLR_VAL(pGPIOC->PUPDR, 0x03u, GPIO_PUPDR_PUPD2_Pos);                // No pull up pull down

	/* LCD WRX_DCX pin */
	REG_SET_VAL(pGPIOD->MODER, 0x01u, 0x03u, GPIO_MODER_MODER13_Pos);       // Output mode
	REG_CLR_BIT(pGPIOD->OTYPER, GPIO_OTYPER_OT13_Pos);                      // Output type output push pull
	REG_SET_VAL(pGPIOD->OSPEEDR, 0x02u, 0x03u, GPIO_OSPEEDR_OSPEED13_Pos);  // Speed as high speed
	REG_CLR_VAL(pGPIOD->PUPDR, 0x03u, GPIO_PUPDR_PUPD13_Pos);               // No pull up pull down

	/* SPI(5) SCK pin PF7 */
	REG_SET_VAL(pGPIOF->MODER, 0x02u, 0x03u, GPIO_MODER_MODER7_Pos);        // Output mode
	REG_CLR_BIT(pGPIOF->OTYPER, GPIO_OTYPER_OT7_Pos);                       // Output type output push pull
	REG_SET_VAL(pGPIOF->OSPEEDR, 0x02u, 0x03u, GPIO_OSPEEDR_OSPEED7_Pos);   // Speed as high speed
	REG_SET_VAL(pGPIOF->AFR[0], 0x05u, 0x0Fu, GPIO_AFRL_AFSEL7_Pos);        // Alternate function 5 (SPI5 SCK)

	/* SPI(5) SDA pin PF9 */
<<<<<<< HEAD
	REG_SET_VAL(pGPIOF->MODER, 0x02u, 0x03u, GPIO_MODER_MODER9_Pos);		// Output mode
	REG_CLR_BIT(pGPIOF->OTYPER, GPIO_OTYPER_OT9_Pos);						// Output type output push pull
	REG_SET_VAL(pGPIOF->OSPEEDR, 0x02u, 0x03u, GPIO_OSPEEDR_OSPEED9_Pos);	// Speed as high speed
	REG_SET_VAL(pGPIOF->AFR[1], 0x05u, 0x0Fu, GPIO_AFRH_AFSEL9_Pos);		// Alternate function 5 (SPI5 DATA)

	/* Lets set the initial state of the RESX, CSX and DCX pins */
	REG_SET_BIT(pGPIOA->ODR, GPIO_ODR_OD7_Pos);                             // Setting RESX pin high
	REG_SET_BIT(pGPIOC->ODR, GPIO_ODR_OD2_Pos);                             // Setting CSX pin high
	REG_SET_BIT(pGPIOD->ODR, GPIO_ODR_OD13_Pos);                            // Setting DCX pin high
=======
	REG_SET_VAL(pGPIOF->MODER, 0x02u, 0x03u, GPIO_MODER_MODER9_Pos);        // Output mode
	REG_CLR_BIT(pGPIOF->OTYPER, GPIO_OTYPER_OT9_Pos);                       // Output type output push pull
	REG_SET_VAL(pGPIOF->OSPEEDR, 0x02u, 0x03u, GPIO_OSPEEDR_OSPEED9_Pos);   // Speed as high speed
	REG_SET_VAL(pGPIOF->AFR[1], 0x05u, 0x0Fu, GPIO_AFRH_AFSEL9_Pos);        // Alternate function 5 (SPI5 DATA)

	/* Lets set the initial state of the RESX, CSX and DCX pins */
	REG_SET_BIT(pGPIOA->ODR, GPIO_ODR_OD7_Pos);                             // Setting RESX pin high
	REG_SET_BIT(pGPIOC->ODR, GPIO_ODR_OD2_Pos);                             // Setting CSX pin high
	REG_SET_BIT(pGPIOD->ODR, GPIO_ODR_OD13_Pos);                            // Setting DCX pin high

>>>>>>> refs/remotes/origin/main
}



void LCD_SPI_Init(void) {
	SPI_TypeDef* pSPI = SPI;
	RCC_TypeDef* pRCC = RCC;

	/* Enable SPI5 peripheral clock */
	REG_SET_BIT(pRCC->APB2ENR, RCC_APB2ENR_SPI5EN_Pos);                     // Enabling APB2 SPI5 peripheral clock

	/* Configure SPI as master in half-duplex MOTOROLA mode */
<<<<<<< HEAD
	REG_SET_BIT(pSPI->CR1, SPI_CR1_BIDIMODE_Pos);							// BIDI mode enable(half-duplex)
	REG_SET_BIT(pSPI->CR1, SPI_CR1_BIDIOE_Pos);                             // In BIDI mode transmit only mode is selected
	REG_CLR_BIT(pSPI->CR1, SPI_CR1_DFF_Pos);								// Data frame format 8 bit is selected
	REG_SET_BIT(pSPI->CR1, SPI_CR1_SSM_Pos);								// Enabling software slave management
	REG_SET_BIT(pSPI->CR1, SPI_CR1_SSI_Pos);								// Setting NSS pin high manually
	REG_CLR_BIT(pSPI->CR1, SPI_CR1_LSBFIRST_Pos);							// Configuring MSB first frame format
	REG_SET_VAL(pSPI->CR1, 0x03, 0x07, SPI_CR1_BR_Pos);						// Configuring SPI baud rate as 5.625MHz(90/16)
	REG_SET_BIT(pSPI->CR1, SPI_CR1_MSTR_Pos);								// Configuring SPI as master
	REG_CLR_BIT(pSPI->CR1, SPI_CR1_CPOL_Pos);								// Setting clock polarity 0(Clock low when idle)
	REG_CLR_BIT(pSPI->CR1, SPI_CR1_CPHA_Pos);								// Setting clock phase 0(Capture in rising edge)
	REG_CLR_BIT(pSPI->CR2, SPI_CR2_FRF_Pos);								// Enabling SPI MOTOROLA mode
=======
	REG_SET_BIT(pSPI->CR1, SPI_CR1_BIDIMODE_Pos);                           // BIDI mode enable(half-duplex)
	REG_CLR_BIT(pSPI->CR1, SPI_CR1_DFF_Pos);                                // Data frame format 8 bit is selected
	REG_SET_BIT(pSPI->CR1, SPI_CR1_SSM_Pos);                                // Enabling software slave management
	REG_SET_BIT(pSPI->CR1, SPI_CR1_SSI_Pos);                                // Setting NSS pin high manually
	REG_CLR_BIT(pSPI->CR1, SPI_CR1_LSBFIRST_Pos);                           // Configuring MSB first frame format
	REG_SET_VAL(pSPI->CR1, 0x03, 0x07, SPI_CR1_BR_Pos);                     // Configuring SPI baud rate as 5.625MHz(90/16)
	REG_SET_BIT(pSPI->CR1, SPI_CR1_MSTR_Pos);                               // Configuring SPI as master
	REG_CLR_BIT(pSPI->CR1, SPI_CR1_CPOL_Pos);                               // Setting clock polarity 0(Clock low when idle)
	REG_CLR_BIT(pSPI->CR1, SPI_CR1_CPHA_Pos);                               // Setting clock phase 0(Capture in rising edge)
	REG_CLR_BIT(pSPI->CR2, SPI_CR2_FRF_Pos);                                // Enabling SPI MOTOROLA mode
>>>>>>> refs/remotes/origin/main
}



void LCD_Reset(void) {
	/* Creating a 50mS reset pulse */
	LCD_RESX_LOW();
	delay_50ms();
	LCD_RESX_HIGH();
	delay_50ms();
}



void LCD_Config(void) {
	uint8_t params[15] = {0};
	LCD_Write_Cmd(ILI9341_SWRESET);
	LCD_Write_Cmd(ILI9341_POWERB);
	params[0] = 0x00;
	params[1] = 0xD9;
	params[2] = 0x30;
	LCD_Write_Data(params, 3);

	LCD_Write_Cmd(ILI9341_POWER_SEQ);
	params[0]= 0x64;
	params[1]= 0x03;
	params[2]= 0X12;
	params[3]= 0X81;
	LCD_Write_Data(params, 4);

	LCD_Write_Cmd(ILI9341_DTCA);
	params[0]= 0x85;
	params[1]= 0x10;
	params[2]= 0x7A;
	LCD_Write_Data(params, 3);

	LCD_Write_Cmd(ILI9341_POWERA);
	params[0]= 0x39;
	params[1]= 0x2C;
	params[2]= 0x00;
	params[3]= 0x34;
	params[4]= 0x02;
	LCD_Write_Data(params, 5);

	LCD_Write_Cmd(ILI9341_PRC);
	params[0]= 0x20;
	LCD_Write_Data(params, 1);

	LCD_Write_Cmd(ILI9341_DTCB);
	params[0]= 0x00;
	params[1]= 0x00;
	LCD_Write_Data(params, 2);

	LCD_Write_Cmd(ILI9341_POWER1);
	params[0]= 0x1B;
	LCD_Write_Data(params, 1);

	LCD_Write_Cmd(ILI9341_POWER2);
	params[0]= 0x12;
	LCD_Write_Data(params, 1);

	LCD_Write_Cmd(ILI9341_VCOM1);
	params[0]= 0x08;
	params[1]= 0x26;
	LCD_Write_Data(params, 2);

	LCD_Write_Cmd(ILI9341_VCOM2);
	params[0]= 0XB7;
	LCD_Write_Data(params, 1);

	uint8_t m;
	m = MADCTL_MV | MADCTL_MY| MADCTL_BGR;

	LCD_Write_Cmd(ILI9341_MAC);    // Memory Access Control <Landscape setting>
	params[0]= m;
	LCD_Write_Data(params, 1);

	LCD_Write_Cmd(ILI9341_PIXEL_FORMAT);
	params[0]= 0x55; //select RGB565
	LCD_Write_Data(params, 1);

	LCD_Write_Cmd(ILI9341_FRMCTR1);
	params[0]= 0x00;
	params[1]= 0x1B;//frame rate = 70
	LCD_Write_Data(params, 2);

	LCD_Write_Cmd(ILI9341_DFC);    // Display Function Control
	params[0]= 0x0A;
	params[1]= 0xA2;
	LCD_Write_Data(params, 2);

	LCD_Write_Cmd(ILI9341_3GAMMA_EN);    // 3Gamma Function Disable
	params[0]= 0x02;
	LCD_Write_Data(params, 1);

	LCD_Write_Cmd(ILI9341_GAMMA);
	params[0]= 0x01;
	LCD_Write_Data(params, 1);

	LCD_Write_Cmd(ILI9341_PGAMMA);    //Set Gamma
	params[0]= 0x0F;
	params[1]= 0x1D;
	params[2]= 0x1A;
	params[3]= 0x0A;
	params[4]= 0x0D;
	params[5]= 0x07;
	params[6]= 0x49;
	params[7]= 0X66;
	params[8]= 0x3B;
	params[9]= 0x07;
	params[10]= 0x11;
	params[11]= 0x01;
	params[12]= 0x09;
	params[13]= 0x05;
	params[14]= 0x04;
	LCD_Write_Data(params, 15);

	LCD_Write_Cmd(ILI9341_NGAMMA);
	params[0]= 0x00;
	params[1]= 0x18;
	params[2]= 0x1D;
	params[3]= 0x02;
	params[4]= 0x0F;
	params[5]= 0x04;
	params[6]= 0x36;
	params[7]= 0x13;
	params[8]= 0x4C;
	params[9]= 0x07;
	params[10]= 0x13;
	params[11]= 0x0F;
	params[12]= 0x2E;
	params[13]= 0x2F;
	params[14]= 0x05;
	LCD_Write_Data(params, 15);

	LCD_Write_Cmd(ILI9341_RASET); //page address set
	params[0]= 0x00;
	params[1]= 0x00;
	params[2]= 0x00;
	params[3]= 0xf0; //240 rows = 0xf0
	LCD_Write_Data(params, 4);

	LCD_Write_Cmd(ILI9341_CASET);
	params[0]= 0x00;
	params[1]= 0x00;
	params[2]= 0x01;
	params[3]= 0x40; //320 columns = 0x140
	LCD_Write_Data(params, 4);

	LCD_Write_Cmd(ILI9341_RGB_INTERFACE);
	params[0] = 0xC2; //Data is fetched during falling edge of DOTCLK
	LCD_Write_Data(params, 1);

	LCD_Write_Cmd(ILI9341_INTERFACE);
	params[0] = 0x00;
	params[1] = 0x00;
	params[2] = 0x06;
	LCD_Write_Data(params, 3);

	LCD_Write_Cmd(ILI9341_SLEEP_OUT); //Exit Sleep
	delay_50ms();
	delay_50ms();
	LCD_Write_Cmd(ILI9341_DISPLAY_ON); //display on
}



void LCD_SPI_Enable(void){
	SPI_TypeDef* pSPI = SPI;
	REG_SET_BIT(pSPI->CR1, SPI_CR1_SPE_Pos);                                // SPI enable
}



void LCD_Write_Cmd(uint8_t cmd) {
	SPI_TypeDef *pSPI = SPI;
	LCD_CSX_LOW();                                                          // CSX pin LOW to initiate SPI communication
	LCD_DCX_LOW();                                                          // DCX pin LOW means command
	while(!REG_READ_BIT(pSPI->SR, SPI_SR_TXE_Pos));                         // Wait until TXE = 1 (Tx buffer is empty)
	REG_WRITE(pSPI->DR, cmd);                                               // Transmitting the command

	// Wait until TXE is 1 (Transmit buffer is empty) and BSY is 0 (SPI is not busy)
	while((!REG_READ_BIT(pSPI->SR, SPI_SR_TXE_Pos)) || (REG_READ_BIT(pSPI->SR, SPI_SR_BSY_Pos)));
	REG_CLR_BIT(pSPI->CR1, SPI_CR1_SPE_Pos);                                // Disabling the SPI peripheral

	LCD_DCX_HIGH();                                                         // After transfer setting DCX pin HIGH
	LCD_CSX_HIGH();                                                         // After transfer setting CSX pin HIGH
}



void LCD_Write_Data(uint8_t *pData, uint32_t len) {
	SPI_TypeDef *pSPI = SPI;
	LCD_CSX_LOW();                                                          // CSX pin LOW to initiate data transfer
	for(uint32_t i = 0; i < len; i++) {
		while(!REG_READ_BIT(pSPI->SR, SPI_SR_TXE_Pos));                     // Wait until TXE = 1 (Tx buffer is empty)
		REG_WRITE(pSPI->DR, pData[i]);                                      // Transmitting command
	}

	// Wait until TXE is 1 (Transmit buffer is empty) and BSY is 0 (SPI is not busy)
	while((!REG_READ_BIT(pSPI->SR, SPI_SR_TXE_Pos)) || (REG_READ_BIT(pSPI->SR, SPI_SR_BSY_Pos)));
	REG_CLR_BIT(pSPI->CR1, SPI_CR1_SPE_Pos);                                // Disabling the SPI peripheral
	LCD_CSX_HIGH();                                                         // After transfer setting CSX pin HIGH
}



/* A software delay of 50ms */
void delay_50ms(void) {
	for(uint32_t i = 0; i < (0xFFFFU * 10U); i++);
}
