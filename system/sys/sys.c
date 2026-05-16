#include "sys.h"

#include "sfrdef.h"
#include "var.h"

__bool SysTick_Init(void) {
	if (AHB_Freq <= 0) { return False; }

	SysTick->LOAD = ((AHB_Freq / 8) / 1000) - 1;  // 选择分频时钟源，所以/8，为1ms
	SysTick->VAL = 0;
	SysTick->CTRL = 0x03;                         // 0b011：AHB/8，启用中断，启用SysTick
	return True;
}

#define TIME_1S    (1000)

volatile __bool timeNeedUpdate = False;      // True表示已过1s，timeAdd需要被调用
volatile __bool displayNeedRefresh = False;  // True表示时间变量已更新，page_time 需要刷新显示
volatile static uint32_t __lastTime = 0;

void SysTick_Handler(void) {
	RunTime++;
	if (RunTime - __lastTime >= TIME_1S) {
		timeNeedUpdate = True;
		__lastTime = RunTime;
	}
	if (sysNB.cb)
		if (RunTime - sysNB.currTime >= sysNB.waitTime)
			isNeedSysNBCallback = True;
}
