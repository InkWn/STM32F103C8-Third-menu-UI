#include "button.h"

#include "var.h"
#include "sfrdef.h"

static GPIO_TypeDef *gpioA = GPIOA;


void Get_Input(void) {
	for (uint8_t i = 0; i < KEY_COUNT; i++) {
		if (!(gpioA->IDR & (0x01 << btnsGroup[i].pin))) {
			btnsGroup[i].pushed = True;
		} else {
			btnsGroup[i].pushed = False;
			btnsGroup[i].handled = False;
		}
	}
}
