#ifndef I2C_H
	#define I2C_H
	
	#include <stdint.h>
	#include "types.h"
	
	typedef enum {
		I2C_STATE_FREE,   // 空闲
		I2C_STATE_START,  // 开始标志
		I2C_STATE_ADDR,   // 发送地址
		I2C_STATE_DATA,   // 发送数据
		I2C_STATE_STOP,   // 结束标志
	} I2C_STATE;
	
	//void __I2C1_Init(uint32_t speed);
	void I2C1_Init(void);
	__bool I2C1_SetAddr(uint8_t addr);
	__bool I2C1_WriteData(uint8_t type, const uint8_t *data, uint16_t len, v_vCallBack cb);

#endif
