#ifndef PAGEMGR_H
	#define PAGEMGR_H
	
	#include "iwidget.h"
	
	void PageMgr_Init(void);
	void PageMgr_SwitchTo(IPageHandle *iPageH);
	//void PageMgr_Show(void);
	void PageMgr_Loop(void);
	__bool PageMgr_UpdateButton(void);
	
#endif
