#include "spi.h"

#include "sfrdef.h"

#define SPI_CR2_RXNEIE  (0x01 << 6)
#define SPI_CR2_TxEIE   (0x01 << 7)

static const uint8_t *sData;           // 数据
static uint16_t sData_len;             // 数据长度
static uint16_t sData_idx;             // 当前索引
static v_vCallBack spiCallback = 0;    // 回调函数

static inline void OLED_CS_LOW(void)  { GPIOA->BRR  = (0x01 << SPI1_CS); }  // 开始通信
static inline void OLED_CS_HIGH(void) { GPIOA->BSRR = (0x01 << SPI1_CS); }  // 结束通信
static inline void OLED_DC_LOW(void)  { GPIOA->BRR  = (0x01 << SPI1_DC); }  // 命令
static inline void OLED_DC_HIGH(void) { GPIOA->BSRR = (0x01 << SPI1_DC); }  // 数据
// 开启/关闭RXNE中断
static inline void SPI1_iRXNE_Enable(void)   { SPI1->CR2 |=  SPI_CR2_RXNEIE; }
static inline void SPI1_iRXNE_Disable(void)  { SPI1->CR2 &= ~SPI_CR2_RXNEIE; }
// 开启/关闭TxE中断
static inline void SPI1_iTxE_Enable(void)   { SPI1->CR2 |=  SPI_CR2_TxEIE; }
static inline void SPI1_iTxE_Disable(void ) { SPI1->CR2 &= ~SPI_CR2_TxEIE; }

void SPI1_Init(void) {
	SPI1->CR1 = 0;
	// 主机模式
	SPI1->CR1 |=  (0x01 << 2);
	// 设置预分配系数为/4，则频率为APB2_Per / 4 = 9MHz   APB2_Per=36000000
	SPI1->CR1 &= ~(0x07 << 3);
	SPI1->CR1 |=  (0x01 << 3);
	// 软件管理
	SPI1->CR1 |=  (0x03 << 8);
	// 使能SPI1
	SPI1->CR1 |=  (0x01 << 6);
}

/**
  * @param type 通信类型 0 = 命令，1 = 数据
  * @param data 数据
  * @param len  数据长度
  * @param cb   CS拉高时的回调函数，需为无返回无参数的函数
  */
__bool SPI1_WriteData(uint8_t type, const uint8_t *data, uint16_t len, v_vCallBack cb) {
	if (!len) return False;
	if (sData) return False;
	
	sData = data;
	sData_len = len;
	sData_idx = 0;
	spiCallback = cb;
	
	OLED_CS_LOW();
	if (type == 0)  OLED_DC_LOW();
	else            OLED_DC_HIGH();
	
	SPI1->DR = sData[0];
	SPI1_iTxE_Enable();
	
	return True;
}

void SPI1_IRQHandler(void) {
	uint16_t sr  = SPI1->SR;
	uint16_t cr2 = SPI1->CR2;

	// 触发RXNE中断且是最后一个字节
	if (sr & (0x01 << 0) && cr2 & SPI_CR2_RXNEIE) {
		(void)SPI1->DR;
		OLED_CS_HIGH();
		SPI1_iRXNE_Disable();
		if (spiCallback) {
			v_vCallBack cb = spiCallback;
			spiCallback = 0;
			cb();
		}
		sData = 0;
	}
	
	// 触发TxE中断且已使能TxE中断
	if (sr & (0x01 << 1) && (SPI1->CR2 & SPI_CR2_TxEIE)) {
		if (++sData_idx < sData_len) {
			SPI1->DR = sData[sData_idx];
		} else {
			// 最后一个数据
			SPI1_iTxE_Disable();
			SPI1_iRXNE_Enable();
		}
	}
}
