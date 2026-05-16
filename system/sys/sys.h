#ifndef SYS_H
	#define SYS_H
	
	#include "types.h"

	__bool SysTick_Init(void);
	extern volatile __bool timeNeedUpdate;
	extern volatile __bool displayNeedRefresh;
	
#endif
