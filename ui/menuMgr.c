#include "menuMgr.h"

#include "sfrdef.h"
#include "oled.h"

// 是否翻页了
static __bool isTurnPage = False;

void Menu_SetIsTurnPage(__bool flag) { isTurnPage = flag; }

// 绘制菜单文本
static void Menu_ShowText(IMenuItem *item) {
	// OLED_PrintLabel(
	// 	OLED_SPI, item->base.geo.posX, item->viewY,
	// 	item->text, item->len, item->font, MODE_NOIID, Horizontal, 0
	// );
	OLED_PrintSuperLabel(
		OLED_SPI, item->base.geo.posX, item->viewY,
		item->text, MODE_NOIID, Horizontal, 0	
	);
}

// 获取一页内最大显示菜单子项的数量
static uint8_t Menu_GetPageCnt(IMenuHandle *iMh) { return iMh->geo.sizeY / iMh->mh; }


__bool Menu_AddMenuItem(IMenuHandle *iMh, IMenuItem *item) {
	if (iMh->begin == NULL) {
		// 链表为空
		iMh->begin = item;
		iMh->end = item;
		iMh->select = item;
		
		iMh->curIdx = 0;
		iMh->topIdx = 0;
		
		item->front = NULL;
		item->next = NULL;
	} else if (iMh->begin == iMh->end) {
		// 链表只有一个菜单
		item->front = iMh->begin;
		item->next = NULL;
		
		iMh->begin->next = item;
		iMh->end = item;
	} else {
		// 多个菜单
		item->front = iMh->end;
		item->next = NULL;
		
		iMh->end->next = item;
		iMh->end = item;
	}

	item->idx = iMh->itemCnt++;
	
	item->base.geo.posX = iMh->geo.posX;
	item->base.geo.posY = iMh->geo.posY + iMh->mh * item->idx;
	
	item->base.geo.sizeX = iMh->geo.sizeX;   // 统一宽度
	item->base.geo.sizeY = iMh->mh;          // 统一高度
	item->base.noSelectDraw = IW_DrawNoSelect;
	item->base.SelectDraw   = IW_DrawSelected;
	// item->base.clickedDraw  = IW_DrawClicked;
	return True;
}

/**
  * @note:  移动光标，也就是移动选中项，仅用得到Up和Down，默认UP
  */
__bool Menu_MoveCursor(IMenuHandle *iMh, KEY_Type key) {
	uint8_t pageCnt = Menu_GetPageCnt(iMh);
	
	if (key == KEY_DOWN) {
		if (iMh->select->next == NULL) return False;
		iMh->select = iMh->select->next;
		
		if (++iMh->curIdx >= iMh->topIdx + pageCnt) {
			if (iMh->topIdx < iMh->itemCnt - pageCnt) {
				iMh->topIdx++;
				isTurnPage = True;
			}
		}
	} else {
		if (iMh->select->front == NULL) return False;
		iMh->select = iMh->select->front;

		if (iMh->curIdx > 0) iMh->curIdx--;
		if (iMh->curIdx < iMh->topIdx) {
			if (iMh->topIdx > 0) {
				iMh->topIdx--;
				isTurnPage = True;
			}
		}
	}
	return True;
}

// 绘制滚动条
void Menu_ShowScroll(IMenuHandle *iMh) {
	uint8_t pageCnt = Menu_GetPageCnt(iMh);
	
	if (iMh->itemCnt <= pageCnt) return;

	uint8_t trackH = iMh->geo.sizeY;
	uint8_t barX   = iMh->geo.posX + iMh->geo.sizeX - 1;

	uint8_t thumbH = (uint16_t)(pageCnt) * trackH / iMh->itemCnt;
	if (thumbH < 1) thumbH = 1;

	uint8_t maxIdx = iMh->itemCnt - pageCnt;
	uint8_t maxY   = trackH - thumbH;
	uint8_t thumbY = (maxIdx > 0) ? (uint16_t)(iMh->topIdx) * maxY / maxIdx : 0;

	OLED_DrawLine(OLED_SPI, barX, 0, barX, trackH - 1, MODE_CLEAR);
	OLED_DrawLine(OLED_SPI, barX, thumbY, barX, thumbY + thumbH - 1, MODE_NORMAL);
}

void Menu_ShowAll(IMenuHandle *iMh) {
	IMenuItem *temp = iMh->begin;
	uint8_t topIdx = iMh->topIdx;
	uint8_t pageCnt = Menu_GetPageCnt(iMh);
	
	if (isTurnPage) {
		OLED_ControlScreen(OLED_SPI, 0);
		isTurnPage = False;
	}

	// 跳过topIdx之前的MenuItem
	for (uint8_t i = 0; i < topIdx; i++) {
		temp = temp->next;
	}
	// 绘制一页的内容
	for (uint8_t i = 0; i < pageCnt; i++) {
		temp->viewY = (temp->idx - topIdx) * iMh->mh;
		Menu_ShowText(temp);
		if (i + topIdx == iMh->curIdx) { IW_DrawSelected((IWidget*)temp); }
		else                           { IW_DrawNoSelect((IWidget*)temp); }
		if (temp->next == NULL) break;
		temp = temp->next;
	}
	Menu_ShowScroll(iMh);
	OLED_Update_SPI1(100);
}
