#ifndef TYPES_H
	#define TYPES_H
	
	#include <stdint.h>

	#define NULL 0

	// 函数指针，无返回无参数
	typedef void (*v_vCallBack)(void);
	
	typedef enum {
		ERROR_NONE,              // 正常状态
		ERROR_I2C_BUSY,          // I2C忙碌
		ERROR_I2C_ADDR_NACK,     // 地址匹配失败
		ERROR_TIME_OUT,          // 超时
		ERROR_UNDEFINE,          // 未定义，一般在OLED的绘制字体处
		ERROR_OVER_FLOW          // 溢出
	} ERROR;
	
	typedef enum {
		False = 0,
		True = 1,
	} __bool;
	
	typedef enum {
		Vertical,
		Horizontal 
	} Orientation;

	typedef enum {
		KEY_LEFT = 0,  // 索引用，不能改成1
		KEY_RIGHT,
		KEY_UP, 
		KEY_DOWN,
		KEY_ENTER,
		KEY_CLOSE,
		KEY_COUNT,     // 表示按键的数量
	} KEY_Type;
	
	typedef enum {
		OLED_SPI,
		OLED_I2C,
	} OLED_Type;

	// 非阻塞延迟回调结构体
	typedef struct {
		volatile uint32_t currTime;   // 当前时间
		volatile uint32_t waitTime;   // 等待时间
		v_vCallBack cb;               // 回调函数
	} SYS_NB_TypeDef;
	
	typedef struct {
		uint8_t pin;                  // 哪个引脚，用来0x01 << pin
		__bool  pushed;               // 是否按下了
		__bool  handled;              // 是否处理了
		__bool  lastRaw;              // 上一次的状态
		__bool  pending;              // 待二次确认，用于消抖
	} Button_TypeDef;

	typedef struct {
		volatile uint32_t CRL;
		volatile uint32_t CRH;
		volatile uint32_t IDR;
		volatile uint32_t ODR;
		volatile uint32_t BSRR;
		volatile uint32_t BRR;
		volatile uint32_t LCKR;
	} GPIO_TypeDef;

	typedef struct {
		volatile uint32_t CR;
		volatile uint32_t CFGR;
		volatile uint32_t CIR;
		volatile uint32_t APB2RSTR;
		volatile uint32_t APB1RSTR;
		volatile uint32_t AHBENR;
		volatile uint32_t APB2ENR;
		volatile uint32_t APB1ENR;
		volatile uint32_t BDCR;
		volatile uint32_t CSR;
	} RCC_TypeDef;

	typedef struct {
		volatile uint32_t CTRL;
		volatile uint32_t LOAD;
		volatile uint32_t VAL;
		volatile uint32_t CALIB;
	} SysTick_TypeDef;

	typedef struct {
		volatile uint32_t CR1;
		volatile uint32_t CR2;
		volatile uint32_t SMCR;
		volatile uint32_t DIER;
		volatile uint32_t SR;
		volatile uint32_t EGR;
		volatile uint32_t CCMR1;
		volatile uint32_t CCMR2;
		volatile uint32_t CCER;
		volatile uint32_t CNT;
		volatile uint32_t PSC;
		volatile uint32_t ARR;
		volatile uint32_t RCR;
		volatile uint32_t CCR1;
		volatile uint32_t CCR2;
		volatile uint32_t CCR3;
		volatile uint32_t CCR4;
		volatile uint32_t BDTR;
		volatile uint32_t DCR;
		volatile uint32_t DMAR;
	} TIM_TypeDef;

	typedef struct {
		volatile uint16_t CR1;
		uint16_t          RESERVED0;
		volatile uint16_t CR2;
		uint16_t          RESERVED1;
		volatile uint16_t OAR1;
		uint16_t          RESERVED2;
		volatile uint16_t OAR2;
		uint16_t          RESERVED3;
		volatile uint16_t DR;
		uint16_t          RESERVED4;
		volatile uint16_t SR1;
		uint16_t          RESERVED5;
		volatile uint16_t SR2;
		uint16_t          RESERVED6;
		volatile uint16_t CCR;
		uint16_t          RESERVED7;
		volatile uint16_t TRISE;
		uint16_t          RESERVED8;
	} I2C_TypeDef;

	typedef struct {
		volatile uint16_t CR1;
		uint16_t          RESERVED0;
		volatile uint16_t CR2;
		uint16_t          RESERVED1;
		volatile uint16_t SR;
		uint16_t          RESERVED2;
		volatile uint16_t DR;
		uint16_t          RESERVED3;
		volatile uint16_t CRCPR;
		uint16_t          RESERVED4;
		volatile uint16_t RXCRCR;
		uint16_t          RESERVED5;
		volatile uint16_t TXCRCR;
		uint16_t          RESERVED6;
		/*
		volatile uint16_t I2SCFGR;
		uint16_t          RESERVED7;
		volatile uint16_t I2SPR;
		uint16_t          RESERVED8;
		*/
	} SPI_TypeDef;

	typedef struct {
		volatile uint32_t ISER[8];
		uint32_t          RESERVED0[24];
		volatile uint32_t ICER[8];
		uint32_t          RESERVED1[24];
		/* 暂时用不到后面的成员
		volatile uint32_t ISPR[8];
		uint32_t          RESERVED2[24];
		volatile uint32_t ICPR[8];
		uint32_t          RESERVED3[24];
		volatile uint32_t IABR[8];
		uint32_t          RESERVED4[56];
		volatile uint8_t  IPR[240];
		*/
	} NVIC_TypeDef;

#endif
