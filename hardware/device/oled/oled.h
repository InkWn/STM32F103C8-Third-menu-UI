#ifndef OLED_H
	#define OLED_H
	
	#include <stdint.h>
	#include "types.h"
	#include "iwidget.h"
	#include "Rstring.h"
	
	typedef enum {
		MODE_NORMAL,    // 正常绘制，默认选项
		MODE_CLEAR,     // 清空模式
		MODE_NOIID,     // 无iid模式
		MODE_NODO,      // 无操作模式，常用于获取geo
	} Draw_Mode;
	
	void i2c_setFree(void);
	void i2c_setBusy(void);
	void spi_setFree(void);
	void spi_setBusy(void);
	
	void OLED_Init(void);
	
	__bool spi_isFree(void);
	__bool i2c_isFree(void);

	void OLED_ControlScreen(OLED_Type oled, uint8_t type);
	__bool OLED_DrawPoint(OLED_Type oled, uint8_t x, uint8_t y, Draw_Mode mode);
	__bool OLED_InvertRegion(OLED_Type oled, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);
	__bool OLED_DrawLine(OLED_Type oled, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, Draw_Mode mode);
	__bool OLED_DrawCircle(OLED_Type oled, uint8_t x, uint8_t y, uint8_t r, Draw_Mode mode);
	__bool OLED_DrawRectangle(OLED_Type oled, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, __bool fill, Draw_Mode mode);

	IWidget OLED_PrintChar(OLED_Type oled, uint8_t x, uint8_t y, const char ch, Font f, Draw_Mode mode);
	IWidget OLED_PrintCN(OLED_Type oled, uint8_t x, uint8_t y, CN_Index cIdx, Font f, Draw_Mode mode);

	IWidget OLED_PrintSuperLabel(
		OLED_Type oled, uint8_t x, uint8_t y,
		MixText *text, Draw_Mode mode,
		Orientation ori, uint8_t spacing
	);
	
	IWidget OLED_PrintLabel(
		OLED_Type oled, uint8_t x, uint8_t y,
		const void *arg, uint8_t len,
		Font f, Draw_Mode mode,
		Orientation ori, uint8_t spacing
	);
	
	IWidget OLED_PrintMixLabel(
		OLED_Type oled, uint8_t x, uint8_t y,
		MixText *text, Draw_Mode mode
	);
	
	IWidget OLED_PrintList(
		OLED_Type oled, uint8_t x, uint8_t y,
		const char *text, uint8_t maxlen, uint8_t words,
		Font f, uint8_t spacing_x, uint8_t spacing_y
	);
	
	__bool OLED_UpdateArea_I2C1(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint32_t timeout);
	__bool OLED_UpdateArea_SPI1(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint32_t timeout);
	__bool OLED_Update_I2C1(uint32_t timeout);
	__bool OLED_Update_SPI1(uint32_t timeout);

#endif
