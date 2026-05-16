#include "oled.h"

#include "sfrdef.h"

#include "i2c.h"
#include "spi.h"

#include "var.h"


// 两个屏幕
uint8_t i2c_screen[8][128];
uint8_t spi_screen[8][128];

// 控件数量，用于赋予id
static uint16_t iWidgetCnts = 0;

// 状态
__bool i2c_free = True;     // i2c空闲
__bool spi_free = True;     // spi空闲

/* ==================== 状态函数 ==================== */
void i2c_setFree(void) { i2c_free = True; }
void i2c_setBusy(void) { i2c_free = False; }
void spi_setFree(void) { spi_free = True; }
void spi_setBusy(void) { spi_free = False; }

__bool i2c_isFree(void) { return i2c_free; }
__bool spi_isFree(void) { return spi_free; }
/* ================================================== */

void OLED_Init(void) {
	// 初始化协议
	I2C1_Init();
	SPI1_Init();
	
	static const uint8_t init_cmds[] = {
		0xAE,           // 关闭显示
		0xD5, 0x80,     // 设置显示时钟分频
		0xA8, 0x3F,     // 设置多路复用率（64行）
		0xD3, 0x00,     // 设置显示偏移（无偏移）
		0x40,           // 设置显示起始行（行0）
		0x8D, 0x14,     // 电荷泵使能（必须！）
		0x20, 0x00,     // 设置内存寻址模式（水平寻址）
		0xA1,           // 设置段重映射
		0xC8,           // 设置 COM 扫描方向
		0xDA, 0x12,     // 设置 COM 引脚硬件配置
		0x81, 0xCF,     // 设置对比度
		0xD9, 0xF1,     // 设置预充电周期
		0xDB, 0x40,     // 设置 VCOMH 取消选择级别
		0xA4,           // 正常显示（使用 RAM 内容）
		0xA6,           // 非反色显示
		0x2E,           // 停止滚动
		0xAF            // 开启显示
	};
	/* ======================= I2C1 =======================*/
	I2C1_SetAddr(0x3C);	
	i2c_setBusy();
	I2C1_WriteData(0, init_cmds, sizeof(init_cmds), i2c_setFree);
	/* ======================= SPI1 =======================*/
	spi_setBusy();
	SPI1_WriteData(0, init_cmds, sizeof(init_cmds), spi_setFree);
	
	OLED_ControlScreen(OLED_I2C, 0);
	OLED_Update_I2C1(100);
	OLED_ControlScreen(OLED_SPI, 0);
	OLED_Update_SPI1(100);
}

/**
  * @param type  0 = 清屏  1 = 满屏
  * @note        无写入屏幕，需update
  */
void OLED_ControlScreen(OLED_Type oled, uint8_t type) {
	if (type != 0) type = 0xFF;
	uint8_t *screen;
	if (oled == OLED_I2C) screen = (uint8_t*)i2c_screen;
	else                  screen = (uint8_t*)spi_screen;

	for (uint16_t i = 0; i < 1024; i++) { *screen++ = type; }
}

uint8_t OLED_GetPix(OLED_Type oled, uint8_t x, uint8_t y) {
	if (x >= OLED_WIDTH || y >= OLED_HEIGHT) return False;  // 边界检查
	
	uint8_t page = y / 8;
	uint8_t bit = y % 8;
	
	uint8_t *screen = (oled == OLED_I2C) ? (uint8_t*)i2c_screen : (uint8_t*)spi_screen;

	uint8_t byte = screen[page * 128 + x];
	return (byte >> bit) & 0x01;
}


__bool OLED_DrawPoint(OLED_Type oled, uint8_t x, uint8_t y, Draw_Mode mode) {
	if (x >= OLED_WIDTH || y >= OLED_HEIGHT) return False;  // 边界检查
	
	uint8_t page = y / 8;
	uint8_t bit = y % 8;
	
	uint8_t *line = (oled == OLED_SPI) ? spi_screen[page]: i2c_screen[page];
	
	if (mode == MODE_CLEAR) { line[x] &= ~(0x01 << bit); }
	else                    { line[x] |=  (0x01 << bit); }
	return True;
}


// 反显区域
__bool OLED_InvertRegion(OLED_Type oled, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2) {
	if (x1 > x2 || y1 > y2) return False;
	if (x1 >= OLED_WIDTH || y1 >= OLED_HEIGHT) return False;
	if (x2 >= OLED_WIDTH) x2 = OLED_WIDTH - 1;
	if (y2 >= OLED_HEIGHT) y2 = OLED_HEIGHT - 1;

	uint8_t start_page = y1 / 8;
	uint8_t end_page   = y2 / 8;
	uint8_t start_bit  = y1 % 8;
	uint8_t end_bit    = y2 % 8;

	uint8_t *screen = (oled == OLED_I2C) ? (uint8_t*)i2c_screen : (uint8_t*)spi_screen;

	for (uint8_t page = start_page; page <= end_page; page++) {
		uint8_t top_bit    = (page == start_page) ? start_bit : 0;
		uint8_t bottom_bit = (page == end_page)   ? end_bit   : 7;
		uint8_t bit_mask   = (0xFF << top_bit) & (0xFF >> (7 - bottom_bit));
	
		uint8_t *line = screen + (page * OLED_WIDTH);
		for (uint8_t col = x1; col <= x2; col++) { line[col] ^= bit_mask; }
	}
	return True;
}


__bool OLED_DrawLine(OLED_Type oled, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, Draw_Mode mode) {
	if (x1 > x2 || y1 > y2) return False;
	if (x1 >= OLED_WIDTH || y1 >= OLED_HEIGHT) return False;
	if (x2 >= OLED_WIDTH) x2 = OLED_WIDTH - 1;
	if (y2 >= OLED_HEIGHT) y2 = OLED_HEIGHT - 1;

	int16_t dx = (x2 > x1) ? (x2 - x1) : (x1 - x2);
	int16_t dy = (y2 > y1) ? (y2 - y1) : (y1 - y2);
	int16_t sx = (x2 > x1) ? 1 : -1;
	int16_t sy = (y2 > y1) ? 1 : -1;
	int16_t err = dx - dy;

	while (1) {
		OLED_DrawPoint(oled, x1, y1, mode);
		if (x1 == x2 && y1 == y2) break;
		
		int16_t e2 = 2 * err;
		if (e2 > -dy) {
			err -= dy;
			x1 += sx;
		}
		if (e2 < dx) {
			err += dx;
			y1 += sy;
		}
	}
	return True;
}

/**
  * @param x   圆心 x 坐标 (0~OLED_WIDTH-1)
  * @param y   圆心 y 坐标 (0~OLED_HEIGHT-1)
  * @param r   半径
  */
__bool OLED_DrawCircle(OLED_Type oled, uint8_t x, uint8_t y, uint8_t r, Draw_Mode mode) {
	if (x >= OLED_WIDTH || y >= OLED_HEIGHT) return False;  // 边界检查
	if (r == 0) {
		OLED_DrawPoint(oled, x, y, mode);  // 半径为零时只画圆心
		return True;
	}

	int16_t dx = 0;        // 当前点的 x 偏移（从 0 到 r）
	int16_t dy = r;        // 当前点的 y 偏移
	int16_t d = 1 - r;     // 决策参数

	while (dx <= dy) {
		// 利用八分对称性绘制八个点
		OLED_DrawPoint(oled, x + dx, y + dy, mode);
		OLED_DrawPoint(oled, x - dx, y + dy, mode);
		OLED_DrawPoint(oled, x + dx, y - dy, mode);
		OLED_DrawPoint(oled, x - dx, y - dy, mode);
		OLED_DrawPoint(oled, x + dy, y + dx, mode);
		OLED_DrawPoint(oled, x - dy, y + dx, mode);
		OLED_DrawPoint(oled, x + dy, y - dx, mode);
		OLED_DrawPoint(oled, x - dy, y - dx, mode);

		if (d < 0) {
			d += 2 * dx + 3;        // 选择东侧点
		} else {
			d += 2 * (dx - dy) + 5; // 选择东南侧点
			dy--;                   // y 偏移减小
		}
		dx++;                       // x 偏移增加
	}
	return True;
}


/**
  * @param x1:   左上角 x 坐标
  * @param y1:   左上角 y 坐标
  * @param x2:   右下角 x 坐标
  * @param y2:   右下角 y 坐标
  * @param fill  是否填充矩形
  */
__bool OLED_DrawRectangle(
	OLED_Type oled,
	uint8_t x1, uint8_t y1,
	uint8_t x2, uint8_t y2,
	__bool fill, Draw_Mode mode
) {
	if (x1 > x2 || y1 > y2) return False;
	if (x1 >= OLED_WIDTH || y1 >= OLED_HEIGHT) return False;
	if (x2 >= OLED_WIDTH)  x2 = OLED_WIDTH - 1;
	if (y2 >= OLED_HEIGHT) y2 = OLED_HEIGHT - 1;

	if (fill) {
		for (uint8_t y = y1; y <= y2; y++) {
			for (uint8_t x = x1; x <= x2; x++) {
				OLED_DrawPoint(oled, x, y, mode);
			}
		}
	} else {
		// 上边
		for (uint8_t x = x1; x <= x2; x++) {
			OLED_DrawPoint(oled, x, y1, mode);
		}
		// 下边
		for (uint8_t x = x1; x <= x2; x++) {
			OLED_DrawPoint(oled, x, y2, mode);
		}
		// 左边
		for (uint8_t y = y1 + 1; y <= y2 - 1; y++) {
			OLED_DrawPoint(oled, x1, y, mode);
		}
		// 右边
		for (uint8_t y = y1 + 1; y <= y2 - 1; y++) {
			OLED_DrawPoint(oled, x2, y, mode);
		}
	}
	return True;
}

/**
  * @note: mode为MODE_CLEAR时，清空fontSize大小的区域，ch参数无效
  */
IWidget OLED_PrintChar(OLED_Type oled, uint8_t x, uint8_t y, const char ch, Font f, Draw_Mode mode) {
	IWidget iw = {
		.geo.posX = x,
		.geo.posY = y,
		.geo.sizeX = f.fW,
		.geo.sizeY = f.fH,
		.iid = -1
	};
	if (mode == MODE_NODO) {
		return iw;
	} else if (mode == MODE_CLEAR) {
		OLED_DrawRectangle(oled, x, y ,x + f.fW, y + f.fH, True, MODE_CLEAR);
		return iw;
	}
	uint8_t *font_bit = NULL;
	uint8_t low  = 0;
	uint8_t high = 0;
	
	switch (f.fontSize) {
		case FONTLIB_EN_0508:
			font_bit = (uint8_t*)ASCII_LETTER_5x8[ch - ASCII_LETTER_5x8_OFFSET];   // 获取字体
			for (uint8_t col = 0; col < 5; col++) {
				for (uint8_t row = 0; row < 8; row++) {
					if (font_bit[col] & (1 << row)) {
						OLED_DrawPoint(oled, x + col, y + row, MODE_NORMAL);
					}
				}
			}
			break;
		case FONTLIB_EN_0816:
			font_bit = (uint8_t*)ASCII_LETTER_8x16[ch - ASCII_LETTER_8x16_OFFSET];
			for (uint8_t col = 0; col < 8; col++) {
				low  = font_bit[col * 2];
				high = font_bit[col * 2 + 1];
				for (uint8_t row = 0; row < 8; row++) {
					if (low  & (1 << row)) {
						OLED_DrawPoint(oled, x + col, y + row, MODE_NORMAL);
					}
					if (high & (1 << row)) {
						OLED_DrawPoint(oled, x + col, y + row + 8, MODE_NORMAL);
					}
				}
			}
			break;
		default:
			gError = ERROR_UNDEFINE;
			return iw;
	}
	if (mode == MODE_NOIID) {
		/* 空操作 */
	} else { iw.iid = iWidgetCnts++; }
	return iw;
}

/**
  * @param cIdx: 汉字在字库里的index，在Rstring里看
  */
IWidget OLED_PrintCN(OLED_Type oled, uint8_t x, uint8_t y, CN_Index cIdx, Font f, Draw_Mode mode) {
	IWidget iw = {
		.geo.posX = x,
		.geo.posY = y,
		.geo.sizeX = f.fW,
		.geo.sizeY = f.fH,
		.iid = -1
	};
	if (mode == MODE_NODO) {
		return iw;
	} else if (mode == MODE_CLEAR) {
		OLED_DrawRectangle(oled, x, y ,x + f.fW, y + f.fH, True, MODE_CLEAR);
		return iw;
	}
	uint8_t *font_bit = NULL;
	uint8_t low  = 0;
	uint8_t high = 0;
	switch (f.fontSize) {
		case FONTLIB_CN_1616:
			iw.geo.sizeX = 16;
			iw.geo.sizeY = 16;
			if (mode == MODE_CLEAR) break;
			
			font_bit = (uint8_t*)CN_16x16[cIdx];
			for (uint8_t col = 0; col < 16; col++) {
				low  = font_bit[col * 2];
				high = font_bit[col * 2 + 1];
				for (uint8_t row = 0; row < 8; row++) {
					if (low  & (1 << row)) {
						OLED_DrawPoint(oled, x + col, y + row, MODE_NORMAL);
					}
					if (high & (1 << row)) {
						OLED_DrawPoint(oled, x + col, y + row + 8, MODE_NORMAL);
					}
				}
			}
			break;
		default:
			gError = ERROR_UNDEFINE;
			return iw;
	}
	if (mode == MODE_NOIID) {
		/* 空操作 */
	} else { iw.iid = iWidgetCnts++; }
	return iw;
}

/**
  * @param oled:     选择I2C OLED或SPI OLED
  * @param x, y:     坐标
  * @param text:     指向 MixText 的数组，必须以 type == MIX_END 的项结尾
  * @param mode:     绘制模式，详情看oled.h
  * @param ori:      文字排列方向
  * @param spacing:  文字间的间隔
  */
IWidget OLED_PrintSuperLabel(
	OLED_Type oled, uint8_t x, uint8_t y,
	MixText *text, Draw_Mode mode,
	Orientation ori, uint8_t spacing
) {
	IWidget iw = {
		.geo.posX = x, .geo.posY = y,
		.geo.sizeX = 0, .geo.sizeY = 0,
		.iid = -1
	};
	if (text == NULL) return iw;

	uint8_t offsetX = 0;
	uint8_t offsetY = 0;
	uint8_t sizeX = 0;
	uint8_t sizeY = 0;

	for (MixText *seg = text; seg->type != MIX_END; seg++) {
		Font *f = &seg->font;           // 当前片段的字体
		uint8_t w = f->fW;              // 字符宽度
		uint8_t h = f->fH;              // 字符高度
		if (ori == Horizontal) {
			if (h > sizeY) sizeY = h;
		} else {
			if (w > sizeX) sizeX = w;
		}
		// 英文
		if (seg->type == MIX_ASCII) {
			const char *str = seg->data.ascii;
			if (str == NULL) continue;
			for (uint8_t i = 0; i < seg->len; i++) {
				OLED_PrintChar(oled, x + offsetX, y + offsetY, *str, *f, MODE_NOIID);
				if (ori == Horizontal)
					offsetX = offsetX + w + spacing;
				else 
					offsetY = offsetY + h + spacing;
				str++;
			}
		// 中文
		} else if (seg->type == MIX_CN) {
			const CN_Index *ci = seg->data.ci;
			if (ci == NULL) continue;
			for (uint8_t i = 0; i < seg->len; i++) {
				OLED_PrintCN(oled, x + offsetX, y + offsetY, *ci, *f, MODE_NOIID);
				if (ori == Horizontal)
					offsetX = offsetX + w + spacing;
				else
					offsetY = offsetY + h + spacing;
				ci++;
			}
		}
	}
	if (ori == Horizontal) {
		iw.geo.sizeX = offsetX - spacing;
		iw.geo.sizeY = sizeY;
	} else {
		iw.geo.sizeX = sizeX;
		iw.geo.sizeY = offsetY - spacing;
	}
	return iw;
}


/**
  * @param oled:     选择I2C OLED或SPI OLED
  * @param x, y:     坐标
  * @param arg:      Chinese时传入CN_Index arg[]，English传入char arg[]
  * @param len:      文本长度，"a"=1, "aa"=2, "菜"=1, "菜单"=2
  * @param f:        字库
  * @param mode:     详情看oled.h
  * @param ori:      文字排列方向
  * @param spacing:  文字间的间隔
  * @note:           只支持纯中文或纯英文
  */
IWidget OLED_PrintLabel(
	OLED_Type oled, uint8_t x, uint8_t y,
	const void *arg, uint8_t len,
	Font f, Draw_Mode mode,
	Orientation ori, uint8_t spacing
) {
	IWidget iw = {
		.geo.posX = x,
		.geo.posY = y,
		.iid = -1
	};
	if (len < 1) {
		gError = ERROR_OVER_FLOW;
		return iw;
	}
	if (f.lang != Chinese && f.lang  != English) {
		gError = ERROR_UNDEFINE;
		return iw;
	}

	uint8_t offset_x = 0;
	uint8_t offset_y = 0;

	// 根据方向定义大小
	if (ori == Horizontal) {
		uint16_t temp_s = (f.fW + spacing) * len - spacing;
		if (temp_s >= OLED_WIDTH) {
			gError = ERROR_OVER_FLOW;
			return iw;
		}
		iw.geo.sizeX = temp_s;
		iw.geo.sizeY = f.fH;
		spacing += f.fW;
	} else {
		uint16_t temp_s = (f.fH + spacing) * len - spacing;
		if (temp_s >= OLED_HEIGHT) {
			gError = ERROR_OVER_FLOW;
			return iw;
		}
		iw.geo.sizeX = f.fW;
		iw.geo.sizeY = temp_s;
		spacing += f.fH;
	}
	// 根据模式判断
	switch (mode) {
		case MODE_NORMAL:
			iw.iid = iWidgetCnts++;
			break;
		case MODE_CLEAR:
			OLED_DrawRectangle(oled, x, y, x + iw.geo.sizeX, y + iw.geo.sizeY, True, MODE_CLEAR);
			return iw;
		case MODE_NOIID: break;
		case MODE_NODO:  return iw;
	}
	// 绘制文本
	if (f.lang == Chinese) {
		for (uint8_t i = 0; i < len; i++) {
			OLED_PrintCN(oled, x + offset_x, y + offset_y, ((const CN_Index*)arg)[i], f, MODE_NOIID);
			if (ori == Horizontal) offset_x += spacing;
			else                   offset_y += spacing;
		}
	} else {
		for (uint8_t i = 0; i < len; i++) {
			OLED_PrintChar(oled, x + offset_x, y + offset_y, ((const char*)arg)[i], f, MODE_NOIID);
			if (ori == Horizontal) offset_x += spacing;
			else                   offset_y += spacing;
		}
	}
	return iw;
}
/**
 * @brief        绘制混合中英文文本（水平排列，无额外间距）
 * @param  oled  屏幕接口
 * @param  x,y   起始坐标
 * @param  text  指向 MixText 数组，必须以 type == MIX_END 的项结尾
 * @param  mode  绘制模式
 * @return       包含全部绘制区域的 IWidget
 */
IWidget OLED_PrintMixLabel(
	OLED_Type oled, uint8_t x, uint8_t y,
	MixText *text, Draw_Mode mode
) {
	IWidget iw = {
		.geo.posX = x, .geo.posY = y,
		.geo.sizeX = 0, .geo.sizeY = 0,
		.iid = -1
	};
	if (text == NULL) return iw;

	uint16_t cur_x = x;
	uint8_t max_height = 0;

	for (MixText *seg = text; seg->type != MIX_END; seg++) {
		Font *f = &seg->font;            // 当前片段的字体
		uint8_t w = f->fW;              // 字符宽度
		uint8_t h = f->fH;              // 字符高度
		if (h > max_height) max_height = h;

		if (seg->type == MIX_ASCII) {
			const char *str = seg->data.ascii;
			if (str == NULL) continue;
			for (uint8_t i = 0; i < seg->len; i++) {
				OLED_PrintChar(oled, cur_x, y, *str, *f, mode);
				cur_x += w;
				str++;
			}
		} else if (seg->type == MIX_CN) {
			const CN_Index *ci = seg->data.ci;
			if (ci == NULL) { continue; }
			for (uint8_t i = 0; i < seg->len; i++) {
				OLED_PrintCN(oled, cur_x, y, *ci, *f, mode);
				cur_x += w;
				ci++;
			}
		}
	}
	iw.geo.sizeX = cur_x - x;
	iw.geo.sizeY = max_height;
	return iw;
}

#if 0
/**
  * @param cIdx_p: 传入一个指向CN_Index的数组
  * @param cnt:    count，字的数量，也就是CN_Index的个数
  */
IWidget OLED_PrintCNS(OLED_Type oled, uint8_t x, uint8_t y, const CN_Index* cIdx_p, uint8_t cnt, uint8_t fontSize) {
	IWidget iw = {
		.geo.posX = x,
		.geo.posY = y,
		.iid = -1
	};
	uint8_t offset = 0;
	uint8_t spacing = 0;
	
	switch (fontSize) {
		case FONT_CN_16x16:
			iw.geo.sizeX = 16 * cnt;
			iw.geo.sizeY = 16;
			spacing = 16;
			break;
		default:
			gError = ERROR_Undefine;
			return iw;
	}
	for (uint8_t i = 0; i < cnt; i++) {
		OLED_PrintCN(oled, x + offset, y, *cIdx_p++, fontSize, 2);
		offset += spacing;
	}
	iw.iid = iWidgetCnts++;
	return iw;
}


/**
  * @param spacing:    字间距
  * @note: 这个不支持中文，中文用OLED_PrintCNS()
  */
IWidget OLED_PrintText(
	OLED_Type oled, uint8_t x, uint8_t y, const char *text,
	uint8_t len, uint8_t fontSize, Orientation ori, uint8_t spacing
) {
	IWidget iw = {
		.geo.posX = x,
		.geo.posY = y,
		.iid = -1
	};
	uint8_t offset = 0;
	
	switch (fontSize) {
		case FONT_LETTER_5x8:
			if (ori == Vertical) {
				iw.geo.sizeX = 5;
				iw.geo.sizeY = (8 + spacing) * len - spacing;
				spacing += 8;
			} else if (ori == Horizontal) {
				iw.geo.sizeX = (5 + spacing) * len - spacing;
				iw.geo.sizeY = 8;
				spacing += 5;   // 字间距+字体大小
			} else {
				gError = ERROR_Undefine;
				return iw;
			}
			break;
		default:
			gError = ERROR_Undefine;
			return iw;
	}
	if (ori == Vertical) {	
		for (uint8_t i = 0; i < len; i++) {
			OLED_PrintChar(oled, x, y + offset, text[i], fontSize, 2);
			offset += spacing;
		}
	} else if (ori == Horizontal) {
		for (uint8_t i = 0; i < len; i++) {
			OLED_PrintChar(oled, x + offset, y, text[i], fontSize, 2);
			offset += spacing;
		}
	} else {
		gError = ERROR_Undefine;
		return iw;
	}
	iw.iid = iWidgetCnts++;
	return iw;
}
#endif

/**
  * @param text:   采用\n来换行，必须以\0结尾
  * @param maxlen: 单词里最长的单词长度，{"AAAA", "BB"}就是4
  * @param words:  单词个数 {"AAA", "BBB"}就是2
  */
IWidget OLED_PrintList(
	OLED_Type oled, uint8_t x, uint8_t y, const char *text,
	uint8_t maxlen, uint8_t words, Font f,
	uint8_t spacing_x, uint8_t spacing_y
) {
	IWidget iw = {
		.geo.posX = x,
		.geo.posY = y,
		.iid = -1
	};
	
	const char *p = text;
	uint8_t offset_x = 0;
	uint8_t offset_y = 0;
	
	switch (f.fontSize) {
		case FONTLIB_EN_0508:
			iw.geo.sizeX = (5 + spacing_x) * maxlen - spacing_x;
			iw.geo.sizeY = (8 + spacing_y) * words - spacing_y;
			spacing_x += 5;
			spacing_y += 8;
			break;
		default:
			gError = ERROR_UNDEFINE;
			return iw;
	}
	for (uint8_t i = 0; i < words; i++) {
		while (*p != '\n' && *p != '\0') {
			OLED_PrintChar(oled, x + offset_x, y + offset_y, *p, f, MODE_NOIID);
			p++;
			offset_x += spacing_x;
		}
		if (*p == '\0') {
			break;
		} else {
			p++;  // 跳过当前\n
			offset_x = 0;
			offset_y += spacing_y;
		}
	}
	iw.iid = iWidgetCnts++;
	return iw;
}

__bool OLED_UpdateArea_I2C1(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint32_t timeout) {
	if (x1 > x2 || y1 > y2) return False;
	if (x1 >= OLED_WIDTH || y1 >= OLED_HEIGHT) return False;
	if (x2 >= OLED_WIDTH)  x2 = OLED_WIDTH - 1;
	if (y2 >= OLED_HEIGHT) y2 = OLED_HEIGHT - 1;

	uint8_t startPage = y1 / 8;
	uint8_t endPage   = y2 / 8;

	uint8_t cmd[] = {
		0x21, x1, x2,              // 列范围
		0x22, startPage, endPage,  // 页范围
	};

	uint32_t tick = RunTime;

	while (!i2c_free) {
		if (RunTime - tick > timeout) {
			gError = ERROR_TIME_OUT;
			return False;
		}
	}
	i2c_free = False;
	I2C1_WriteData(0, cmd, sizeof(cmd), i2c_setFree);

	while (!i2c_free) {
		if (RunTime - tick > timeout) {
			gError = ERROR_TIME_OUT;
			return False;
		}
	}

	for (uint8_t page = startPage; page <= endPage; page++) {
		while (!i2c_free) {
			if (RunTime - tick > timeout) {
				gError = ERROR_TIME_OUT;
				return False;
			}
		}
		i2c_free = False;
		uint8_t *pData = &i2c_screen[page][x1];
		I2C1_WriteData(1, pData, x2 - x1 + 1, i2c_setFree);
	}
	return True;
}

__bool OLED_UpdateArea_SPI1(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint32_t timeout) {
	if (x1 > x2 || y1 > y2) return False;
	if (x1 >= OLED_WIDTH || y1 >= OLED_HEIGHT) return False;
	if (x2 >= OLED_WIDTH)  x2 = OLED_WIDTH - 1;
	if (y2 >= OLED_HEIGHT) y2 = OLED_HEIGHT - 1;

	uint8_t startPage = y1 / 8;
	uint8_t endPage   = y2 / 8;

	uint8_t cmd[] = {
		0x21, x1, x2,              // 列范围
		0x22, startPage, endPage,  // 页范围
	};

	uint32_t tick = RunTime;

	while (!spi_free) {
		if (RunTime - tick > timeout) {
			gError = ERROR_TIME_OUT;
			return False;
		}
	}
	spi_free = False;
	SPI1_WriteData(0, cmd, sizeof(cmd), spi_setFree);

	while (!spi_free) {
		if (RunTime - tick > timeout) {
			gError = ERROR_TIME_OUT;
			return False;
		}
	}

	for (uint8_t page = startPage; page <= endPage; page++) {
		while (!spi_free) {
			if (RunTime - tick > timeout) {
				gError = ERROR_TIME_OUT;
				return False;
			}
		}
		spi_free = False;
		uint8_t *pData = &spi_screen[page][x1];
		SPI1_WriteData(1, pData, x2 - x1 + 1, spi_setFree);
	}
	return True;
}


// 全填充命令
static const uint8_t cmd_fill[] = {
	0x21, 0x00, 0x7F,   // 设置列地址范围 0-127
	0x22, 0x00, 0x07    // 设置页地址范围 0-7
};

/**
  * @param timeout  设定时间限制，单位ms
  * @note           会阻塞线程，发送完 更新屏幕的数据 后不会等待发送完毕，直接返回True
  * @return         超时返回False，正常返回True
  */
__bool OLED_Update_I2C1(uint32_t timeout) {
	uint32_t tick = RunTime;
	
	while (!i2c_free) {
		if (RunTime - tick > timeout) {
			gError = ERROR_TIME_OUT;
			return False;
		}
	}
	
	i2c_free = False;
	I2C1_WriteData(0, cmd_fill, sizeof(cmd_fill), i2c_setFree);
	
	while (!i2c_free) {
		if (RunTime - tick > timeout) {
			gError = ERROR_TIME_OUT;
			return False;
		}
	}
	
	i2c_free = False;
	I2C1_WriteData(1, (const uint8_t*)i2c_screen, 1024, i2c_setFree);
	return True;
}

__bool OLED_Update_SPI1(uint32_t timeout) {
	uint32_t tick = RunTime;
	
	while (!spi_free) {
		if (RunTime - tick > timeout) {
			gError = ERROR_TIME_OUT;
			return False;
		}
	}
	
	spi_free = False;
	SPI1_WriteData(0, cmd_fill, sizeof(cmd_fill), spi_setFree);
	
	while (!spi_free) {
		if (RunTime - tick > timeout) {
			gError = ERROR_TIME_OUT;
			return False;
		}
	}
	
	spi_free = False;
	SPI1_WriteData(1, (const uint8_t*)spi_screen, 1024, spi_setFree);
	return True;
}
