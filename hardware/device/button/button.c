#include "button.h"

#include "var.h"
#include "sfrdef.h"

static GPIO_TypeDef *gpioA = GPIOA;

void Button_Input_Check(void) {
	for (uint8_t i = 0; i < KEY_COUNT; i++) {
		__bool raw = (__bool)!(gpioA->IDR & (0x01 << btnsGroup[i].pin));
		if ((raw != btnsGroup[i].lastRaw)) {
			// 改变了状态
			btnsGroup[i].lastRaw = raw;
			btnsGroup[i].pending = True;
		}
	}
}

void Button_Input_Confirm(void) {
	for (uint8_t i = 0; i < KEY_COUNT; i++) {
		if (!btnsGroup[i].pending) continue;

		__bool raw = (__bool)!(gpioA->IDR & (0x01 << btnsGroup[i].pin));
		if (raw == btnsGroup[i].lastRaw) {
			btnsGroup[i].pushed = raw;
			if (!raw) btnsGroup[i].handled = False;
			btnsGroup[i].pending = False;
		} else {
			btnsGroup[i].lastRaw = raw;
		}
	}
}
