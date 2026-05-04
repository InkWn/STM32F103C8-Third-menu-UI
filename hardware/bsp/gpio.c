#include "gpio.h"
#include "sfrdef.h"

void LED_GPIO_Init(void) {
	GPIOC->CRH &= ~(0x0FUL << ((LED - 8) * 4UL));
	GPIOC->CRH |=  (0x01UL << ((LED - 8) * 4UL));
	GPIOC->BSRR =  (0x01UL << LED);  // 关灯
}

void BUTTON_GPIO_Init(void) {
	// 0b1000: 上下拉输入模式
	GPIOA->CRH &= ~(0x0FUL << (Btn_Left   - 8) * 4UL);
	GPIOA->CRH &= ~(0x0FUL << (Btn_Right  - 8) * 4UL);
	GPIOA->CRH &= ~(0x0FUL << (Btn_Up     - 8) * 4UL);
	GPIOA->CRH &= ~(0x0FUL << (Btn_Bottom - 8) * 4UL);
	GPIOA->CRH &= ~(0x0FUL << (Btn_Enter  - 8) * 4UL);
	GPIOA->CRH &= ~(0x0FUL << (Btn_Close  - 8) * 4UL);
	
	GPIOA->CRH |=  (0x08UL << (Btn_Left   - 8) * 4UL);
	GPIOA->CRH |=  (0x08UL << (Btn_Right  - 8) * 4UL);
	GPIOA->CRH |=  (0x08UL << (Btn_Up     - 8) * 4UL);
	GPIOA->CRH |=  (0x08UL << (Btn_Bottom - 8) * 4UL);
	GPIOA->CRH |=  (0x08UL << (Btn_Enter  - 8) * 4UL);
	GPIOA->CRH |=  (0x08UL << (Btn_Close  - 8) * 4UL);
	// 上拉电阻
	GPIOA->BSRR |=  (0x01UL << Btn_Left);
	GPIOA->BSRR |=  (0x01UL << Btn_Right);
	GPIOA->BSRR |=  (0x01UL << Btn_Up);
	GPIOA->BSRR |=  (0x01UL << Btn_Bottom);
	GPIOA->BSRR |=  (0x01UL << Btn_Enter);
	GPIOA->BSRR |=  (0x01UL << Btn_Close);
}

void I2C1_GPIO_Init(void) {
	// 两个脚都是低位的
	GPIOB->CRL &= ~(0x0FUL << (I2C1_SCL * 4UL));
	GPIOB->CRL &= ~(0x0FUL << (I2C1_SDA * 4UL));
	// 都配置为复用开漏10MHz
	GPIOB->CRL |=  (0x0DUL << (I2C1_SCL * 4UL));
	GPIOB->CRL |=  (0x0DUL << (I2C1_SDA * 4UL));
}

void SPI1_GPIO_Init(void) {
	// 都是低位，直接CRL
	GPIO_TypeDef *gpioA = GPIOA;
	gpioA->CRL &= ~(0x0FUL << (SPI1_DC   * 4UL));
	gpioA->CRL &= ~(0x0FUL << (SPI1_CS   * 4UL));
	gpioA->CRL &= ~(0x0FUL << (SPI1_SCK  * 4UL));
	gpioA->CRL &= ~(0x0FUL << (SPI1_MOSI * 4UL));
	// 通用推挽 10MHz
	gpioA->CRL |= (0x03UL << (SPI1_DC * 4UL));
	gpioA->CRL |= (0x03UL << (SPI1_CS * 4UL));
	// 复用推挽 10MHz
	gpioA->CRL |= (0x0BUL << (SPI1_SCK  * 4UL));
	gpioA->CRL |= (0x0BUL << (SPI1_MOSI * 4UL));
}

void GPIO_Init(void) {
	LED_GPIO_Init();
	BUTTON_GPIO_Init();
	SPI1_GPIO_Init();
	I2C1_GPIO_Init();
}
