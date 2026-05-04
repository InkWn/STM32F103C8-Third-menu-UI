#ifndef Rstring_h
	#define Rstring_h
	
	#include <stdint.h>

	#define ASCII_LETTER_5x8_OFFSET    32  // LETTER 5x8 字库的偏移量
	#define ASCII_LETTER_8x16_OFFSET   32
	
	typedef enum {
		Chinese = 1,
		English,
		Mixed         // 混合型
	} Language;

	typedef enum {
		CN_CAI_001    = 0,    // 菜，001用来给同音字拓展
		CN_DAN_001    = 1,    // 单
		CN_SHI_001    = 2,    // 时
		CN_JIAN_001   = 3,    // 间
		CN_XUAN_001   = 4,    // 选
		CN_XIANG_001  = 5,    // 项
		CN_FAN_001    = 6,    // 返
		CN_HUI_001    = 7,    // 回
		CN_YI_001,            // 一到五
		CN_ER_001,
		CN_SAN_001,
		CN_SI_001,
		CN_WU_001
	} CN_Index;
	
	typedef enum {
		FONTLIB_EN_0508,
		FONTLIB_EN_0816,
		FONTLIB_CN_1616
	} FontLib;
	
	typedef struct {
		FontLib fontSize;     // 字体库
		Language lang;        // 语言
		uint8_t fW, fH;       // 字体的长宽，例如FONT_EN_0508 = 5, 8
	} Font;

	// 混合字体，适合中英一起输出到屏幕
	typedef enum {
		MIX_ASCII = 0,
		MIX_CN,
		MIX_END     // 数组终止标记
	} MixSegType;

	typedef struct MixText {
		Font font;
		MixSegType type;
		union {
			const char *ascii;
			CN_Index *ci;
		} data;
		uint8_t len;
	} MixText;
	
	extern const Font FONT_EN_0508;
	extern const Font FONT_EN_0816;
	extern const Font FONT_CN_1616;

	extern const uint8_t ASCII_LETTER_5x8[][5];
	extern const uint8_t ASCII_LETTER_8x16[][16];
	extern const uint8_t CN_16x16[][32];
	
#endif
