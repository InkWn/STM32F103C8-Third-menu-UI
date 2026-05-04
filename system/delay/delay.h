#ifndef DELAY_H
	#define DELAY_H
	
	#include <stdint.h>
	#include "types.h"

	void delay_ms(uint32_t ms);
	void delay_s(uint32_t s);
	__bool delay_nb(uint32_t ms, v_vCallBack fun);

#endif
