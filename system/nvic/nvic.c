#include "nvic.h"
#include "sfrdef.h"

// I2C1_EV 使能中断 31
void NVIC_I2C1_EV_Init(void) { NVIC->ISER[0] |= (0x01UL << 31UL); }
// I2C1_ER 使能中断 32
void NVIC_I2C1_ER_Init(void) { NVIC->ISER[1] |= (0x01UL << 0UL); }
// SPI1    使能中断 35
void NVIC_SPI1_Init(void)    { NVIC->ISER[1] |= (0x01UL << 3UL); }
// USART   使能中断 39
void NVIC_USART_Init(void)   { NVIC->ISER[1] |= (0x01UL << 7UL); }

void NVIC_Init(void) {
	NVIC_SPI1_Init();
	NVIC_I2C1_EV_Init();
	NVIC_I2C1_ER_Init();
	NVIC_USART_Init();
}
