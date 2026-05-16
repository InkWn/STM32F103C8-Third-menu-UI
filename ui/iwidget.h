#ifndef IWIDGET_H
	#define IWIDGET_H
	
	#include <stdint.h>
	#include "types.h"
	#include "Rstring.h"
	
	/**************************************************/
	// 几何结构体
	#define FullScreen   { 0, 0, OLED_WIDTH, OLED_HEIGHT }

	typedef struct IGeometry {
		uint8_t posX, posY;    // 控件初始位置
		uint8_t sizeX, sizeY;  // 控件大小
	} IGeometry;
	
	// 基础控件
	typedef struct IWidget {
		IGeometry geo;
		void (*noSelectDraw)(struct IWidget*);   // 未选中 绘制函数
		void (*SelectDraw)(struct IWidget*);     // 选中   绘制函数
		void (*clickedDraw)(struct IWidget*);    // 点击   绘制函数
		int16_t iid;                             // 控件id，-1为无效控件
		__bool isInvert;                         // 是否已反显
	} IWidget;

	// ！代表初始化时得赋值	
	typedef struct IButton {
		IWidget base;
		void (*enterCall)(void);
		void (*leaveCall)(void);
		Font font;                   // ！用的字库
		const void *text;            // ！文本
		uint8_t len;                 // ！文本长度
		uint8_t idx;                 // ！控件索引
	} IButton;

	// 菜单子项
	typedef struct IMenuItem {
		IWidget base;
		void (*enterCall)(void);
		void (*leaveCall)(void);
		struct IMenuItem* front;
		struct IMenuItem* next;		

		MixText *text;               // ！文本
		uint8_t idx;                 // 在菜单中的序号
		uint8_t viewX;               // 暂时用不到
		uint8_t viewY;               // 绘制时在Y轴的坐标
	} IMenuItem;
	
	// 菜单头部
	typedef struct IMenuHandle {
		IGeometry geo;               // ！几何信息
		struct IMenuItem* begin;     // 开头的菜单项
		struct IMenuItem* end;       // 末尾的菜单项
		struct IMenuItem* select;    // 当前选中项
		
		uint8_t mh;                  // ！menu height, 每个菜单项的高度
		uint8_t curIdx;              // 当前选中的索引
		uint8_t topIdx;              // 菜单绘制时最顶部的菜单子项的索引
		uint8_t itemCnt;             // 菜单数量
	} IMenuHandle;
	
	typedef struct IPageHandle {
		IGeometry geo;

		void (*initFunc)(void);      // 初始化函数，仅调用一次，里面不能调用刷新屏幕的函数
		void (*loopFunc)(void);      // 循环函数，自行处理按键
		void (*showFunc)(void);      // 从别的page切换回来时调用，统一清屏，无需在该函数里编写，但需手动更新屏幕
		void (*exitFunc)(void);      // 关闭该页面时调用，可做关闭前处理
	} IPageHandle;
	
	void IW_DrawSelected(IWidget *iw);
	void IW_DrawNoSelect(IWidget *iw);
	// void IW_DrawClicked(IWidget *iw);

#endif
