#ifndef SFRDEF_H
	#define SFRDEF_H

	#include "types.h"
	
	#define PERIPH_BASE       0x40000000UL
	#define APB1PERIPH_BASE   (PERIPH_BASE + 0x00000000UL)
	#define APB2PERIPH_BASE   (PERIPH_BASE + 0x00010000UL)
	#define AHBPERIPH_BASE    (PERIPH_BASE + 0x00020000UL)

	#define I2C1_BASE         (APB1PERIPH_BASE + 0x00005400UL)
	
	#define GPIOA_BASE        (APB2PERIPH_BASE + 0x00000800UL)
	#define GPIOB_BASE        (APB2PERIPH_BASE + 0x00000C00UL)
	#define GPIOC_BASE        (APB2PERIPH_BASE + 0x00001000UL)
	#define SPI1_BASE         (APB2PERIPH_BASE + 0x00003000UL)

	#define RCC_BASE          (AHBPERIPH_BASE  + 0x00001000UL)
	
	#define SYSTICK_BASE      0xE000E010UL
	#define NVIC_BASE         0xE000E100UL
	
	// 使能
	#define RCC_GPIOA_ENABLE  (0x01 << 2)    // RCC使能GPIOA  APB2
	#define RCC_GPIOB_ENABLE  (0x01 << 3)    // RCC使能GPIOB  APB2
	#define RCC_GPIOC_ENABLE  (0x01 << 4)    // RCC使能GPIOC  APB2
	#define RCC_SPI1_ENABLE   (0x01 << 12)   // RCC使能SPI1   APB2
	#define RCC_I2C1_ENABLE   (0x01 << 21)   // RCC使能I2C1   APB1

	// 引脚
	#define LED               13UL           // PC13
	
	#define I2C1_SCL          6UL            // PB6
	#define I2C1_SDA          7UL            // PB7
	
	#define SPI1_DC           3UL            // PA3
	#define SPI1_CS           4UL            // PA4
	#define SPI1_SCK          5UL            // PA5
	#define SPI1_MOSI         7UL            // PA7
	
	// 输入引脚，定义高电平为真
	#define Btn_Left          8UL            // PA8
	#define Btn_Right         9UL            // PA9
	#define Btn_Up            10UL           // PA10
	#define Btn_Bottom        11UL           // PA11
	#define Btn_Enter         12UL           // PA12
	#define Btn_Close         15UL           // PA15
	
	// OLED尺寸
	#define OLED_WIDTH        128U
	#define OLED_HEIGHT       64U

	// 宏定义
	#define I2C1           ((I2C_TypeDef *)I2C1_BASE)
	#define GPIOA          ((GPIO_TypeDef *)GPIOA_BASE)
	#define GPIOB          ((GPIO_TypeDef *)GPIOB_BASE)
	#define GPIOC          ((GPIO_TypeDef *)GPIOC_BASE)
	#define SPI1           ((SPI_TypeDef *)SPI1_BASE)
	#define RCC            ((RCC_TypeDef *)RCC_BASE)
	#define SysTick        ((SysTick_TypeDef*)SYSTICK_BASE)
	#define NVIC           ((NVIC_TypeDef *)NVIC_BASE)

#endif
