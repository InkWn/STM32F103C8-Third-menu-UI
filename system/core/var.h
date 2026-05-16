#ifndef VAR_H
	#define VAR_H
	
	#include <stdint.h>
	#include "types.h"
	#include "sfrdef.h"
	
	extern volatile uint32_t RunTime;
	
	extern uint32_t AHB_Freq;
	extern uint32_t APB1_Per_Freq;
	extern uint32_t APB1_TIM_Freq;
	extern uint32_t APB2_Per_Freq;
	
	extern ERROR  gError;
	
	// extern volatile uint8_t RF_Count;
	extern SYS_NB_TypeDef sysNB;
	extern volatile __bool isNeedSysNBCallback;
	
	extern Button_TypeDef btnsGroup[KEY_COUNT];
	
	extern uint8_t time_h;
	extern uint8_t time_m;
	extern uint8_t time_s;

#endif
