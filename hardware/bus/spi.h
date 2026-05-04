#ifndef SPI_H
	#define SPI_H
	
	#include <stdint.h>
	#include "types.h"

	void SPI1_Init(void);
	__bool SPI1_WriteData(uint8_t type, const uint8_t *data, uint16_t len, v_vCallBack cb);	

#endif
