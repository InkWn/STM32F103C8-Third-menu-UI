#include "var.h"

volatile uint32_t RunTime = 0;                // 系统运行时间
uint32_t AHB_Freq = 0;                        // AHB时钟频率
uint32_t APB1_Per_Freq = 0;                   // APB1外设时钟频率
uint32_t APB1_TIM_Freq = 0;                   // APB1_TIM时钟频率
uint32_t APB2_Per_Freq = 0;                   // APB2外设时钟频率

ERROR gError = ERROR_NONE;                    // 全局错误管理

// volatile uint8_t RF_Count = 0;             // 回调函数的个数，不可超过256
SYS_NB_TypeDef sysNB = {0};                   // sys非阻塞延迟结构体当前指针
volatile __bool isNeedSysNBCallback = False;  // sys回调需要被调用

Button_TypeDef btnsGroup[KEY_COUNT] = {
	{.pin = Btn_Left,   .idx = 0},
	{.pin = Btn_Right,  .idx = 1},
	{.pin = Btn_Up,     .idx = 2},
	{.pin = Btn_Bottom, .idx = 3},
	{.pin = Btn_Enter,  .idx = 4},
	{.pin = Btn_Close,  .idx = 5},
};

uint8_t time_h = 0;
uint8_t time_m = 0;
uint8_t time_s = 0;
