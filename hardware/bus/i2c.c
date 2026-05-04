#include "i2c.h"

#include "var.h"
#include "sfrdef.h"

static I2C_STATE i2c_state = I2C_STATE_FREE;

static uint8_t iAddr = 0;              // 地址
static uint8_t iType = 0;              // 通信类型
static const uint8_t *iData;           // 待发送的数据
static uint16_t iData_len;             // 数据长度
static uint16_t iData_idx;             // 数据发送索引
static v_vCallBack i2cCallback = 0;    // 回调函数
// 开启/关闭通信
static inline void I2C1_Start(void) { I2C1->CR1 |= (0x01 << 8); }
static inline void I2C1_Stop(void)  { I2C1->CR1 |= (0x01 << 9); }
// 使能/关闭所有中断
static inline void I2C1_iEnable(void)  { I2C1->CR2 |=  (0x07 << 8); }
static inline void I2C1_iDisable(void) { I2C1->CR2 &= ~(0x07 << 8); }

#define I2C1_Free() do {                              \
						i2c_state = I2C_STATE_FREE;   \
						iData = 0;                    \
						iData_len = 0;                \
						iData_idx = 0;                \
						i2cCallback = 0;              \
					} while(0);

/**
  * @param speed 目标速度
  */
void __I2C1_Init(uint32_t speed) {
	// 设置FREQ
	I2C1->CR2 = APB1_Per_Freq / 1000000;
	// 设置为快速并清其他位
	I2C1->CCR = (0x01 << 15);
	// 计算 CCR 值
	I2C1->CCR |= (APB1_Per_Freq / (3 * speed));
	// 配置TRISE
	I2C1->TRISE = (300 * (APB1_Per_Freq / 1000000) / 1000) + 1;
	// 使能 I2C1事件 中断
	I2C1->CR2 |= (0x01 << 10);
	// 使能I2C
	I2C1->CR1 |= (0x01);
}

void I2C1_Init(void) { __I2C1_Init(400000); }

__bool I2C1_SetAddr(uint8_t addr) {
	if (i2c_state != I2C_STATE_FREE) return False;  // 忙碌
	iAddr = (addr << 1) | 0;
	return True;
}

/**
  * @param type 通信类型 0 = 命令，1 = 数据
  * @param data 数据指针
  * @param len  数据长度
  * @param cb   数据Stop时的回调函数，需为无返回无参数的函数
  */
__bool I2C1_WriteData(uint8_t type, const uint8_t *data, uint16_t len, v_vCallBack cb) {
	if (!iAddr || !len) return False;
	if (i2c_state != I2C_STATE_FREE) return False;
	
	iType =  type;
	iData = data;
	iData_len = len;
	iData_idx = 0;
	i2cCallback = cb;
	
	i2c_state = I2C_STATE_START;
	
	I2C1_iEnable();
	I2C1_Start();
	
	return True;
}

void I2C1_EV_IRQHandler(void) {
	uint16_t sr = I2C1->SR1;
	// SB
	if ((sr & (0x01 << 0)) && (i2c_state == I2C_STATE_START)) {
		I2C1->DR = iAddr;
		i2c_state = I2C_STATE_ADDR;
	}
	// ADDR
	if ((sr & (0x01 << 1)) && (i2c_state == I2C_STATE_ADDR)) {
		(void)I2C1->SR2;  // 读取以清除ADDR
		i2c_state = I2C_STATE_DATA;
		if (iType == 0)   { I2C1->DR = 0x00; }
		else              { I2C1->DR = 0x40; }
	}
	// BTF，数据发送完成
	if ((sr & (0x01 << 2)) && (i2c_state == I2C_STATE_STOP)) {
		if (iData_idx == iData_len) {  // 最后一个
			I2C1_Stop();
			if (i2cCallback) {
				v_vCallBack cb = i2cCallback;
				i2cCallback = 0;
				cb();
			}
			I2C1_iDisable();
			I2C1_Free();
		}
	}
	// TxE，数据为空，写入下一个
	if ((sr & (0x01 << 7)) && (i2c_state == I2C_STATE_DATA)) {
		if (iData_idx < iData_len) {
			I2C1->DR = iData[iData_idx++];
		} else { i2c_state = I2C_STATE_STOP; }
	}
}

void I2C1_ER_IRQHandler(void) {
	// AF，地址无应答
	if (I2C1->SR1 & (0x01 << 10)) {
		I2C1_Stop();
		I2C1->SR1 &= ~(0x01 << 10);
		gError = ERROR_I2C_ADDR_NACK;
	}
}
