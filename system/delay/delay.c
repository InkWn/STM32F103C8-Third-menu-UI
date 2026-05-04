#include "delay.h"
#include "var.h"

// 阻塞延时 ms
void delay_ms(uint32_t ms) {
	uint32_t nowTime = RunTime;
	while ((RunTime - nowTime) < ms);
}

// 阻塞延时 s
void delay_s(uint32_t s) {
	delay_ms(s * 1000);
}

// Non-blocking，非阻塞延时
__bool delay_nb(uint32_t ms, v_vCallBack fun) {
	__disable_irq();
	if (sysNB.cb) {
		__enable_irq();
		return False;
	}
	sysNB.waitTime = ms;
	sysNB.currTime = RunTime;
	sysNB.cb = fun;
	__enable_irq();
	return True;
}
