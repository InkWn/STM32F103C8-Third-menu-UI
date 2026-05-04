#ifndef MENUMGR_H
	#define MENUMGR_H
	
	#include "types.h"
	#include "iwidget.h"

	void Menu_SetIsTurnPage(__bool flag);
	__bool Menu_AddMenuItem(IMenuHandle *iMh, IMenuItem *item);
	__bool Menu_MoveCursor(IMenuHandle *iMh, KEY_Type key);
	//void Menu_Click(IMenuHandle *iMh);
	void Menu_ShowAll(IMenuHandle *iMh);
	
#endif
