#include "rcc.h"

#include "sfrdef.h"
#include "var.h"

/**
  * @brief  使用HSI-PLL，PLLMUL=x9，AHB=36MHz，APB1,APB2=36MHz
  */
void RCC_Init(void) {
	RCC_TypeDef *rcc = RCC;
	/* 开启HSI并等待 */ 
	rcc->CR |= (0x01 << 0);              
	while (!(rcc->CR & (0x01 << 1UL)));
	/* PLL选择HSI时钟源 */
	rcc->CFGR &= ~(0x01 << 16);
	/* PLLMUL == x9 */
	rcc->CFGR &= ~(0x0F << 18);
	rcc->CFGR |=  (0x07 << 18);
	/* 开启PLL并等待 */
	rcc->CR |= (0x01 << 24UL);
	while (!(rcc->CR & (0x01 << 25UL)));
	/* 选择HSI-PLL并等待 */
	rcc->CFGR &= ~(0x03 << 0);
	rcc->CFGR |=  (0x02 << 0);
	while ((rcc->CFGR & (0x03 << 2)) != (0x02 << 2));
	/* AHB  == /1 */
	rcc->CFGR &= ~(0x0F << 4);
	/* APB1 == /1 */
	rcc->CFGR &= ~(0x07 << 8);
	/* 使能外设 */
	rcc->APB2ENR |= RCC_GPIOA_ENABLE;
	rcc->APB2ENR |= RCC_GPIOB_ENABLE;
	rcc->APB2ENR |= RCC_GPIOC_ENABLE;
	rcc->APB2ENR |= RCC_SPI1_ENABLE;
	rcc->APB1ENR |= RCC_I2C1_ENABLE;
	/* 计算频率 */
	// HSI=8，HSI-PLL分支默认/2，PLL x 9，== 36M
	AHB_Freq = 36000000;
	// APB1分频/1，则 APB1_Per_Freq = AHB_Freq / 1 == 36M
	APB1_Per_Freq = 36000000;
	// APB1分频 == /1，TIM = APB1_Per_Freq * 1
	APB1_TIM_Freq = 36000000;
	// APB2分频 == /1，则 APB2_Per_Freq = AHB_Freq / 1
	APB2_Per_Freq = 36000000;
}
