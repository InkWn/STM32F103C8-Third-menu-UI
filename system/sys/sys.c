#include "sys.h"

#include "sfrdef.h"
#include "var.h"

#define TIME_1S    (1000)

__bool SysTick_Init(void) {
	if (AHB_Freq <= 0) { return False; }

	SysTick->LOAD = ((AHB_Freq / 8) / 1000) - 1;  // 选择分频时钟源，所以/8，为1ms
	SysTick->VAL = 0;
	SysTick->CTRL = 0x03;                         // 0b011：AHB/8，启用中断，启用SysTick
	return True;
}

/*
已经过了1s。
为2时：主循环改变h,m,s变量，然后-=1；
为1时：page_time.c内部更新字体，然后-=1；
为0时：待机
*/
volatile uint8_t is1000ms = 0;
volatile static uint32_t __lastTime = 0;

void SysTick_Handler(void) {
	RunTime++;
	if (RunTime - __lastTime >= TIME_1S) {
		is1000ms = 2;
		__lastTime = RunTime;
	}
	if (sysNB.cb) {
		if (RunTime - sysNB.currTime >= sysNB.waitTime) {
			v_vCallBack cb = sysNB.cb;
			sysNB.cb = 0;
			cb();  // 这个得改，移到main的主循环里，不然严重影响时间
		}
	}
}
