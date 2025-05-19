#ifndef GUARD_BAG_H
#define GUARD_BAG_H

#include "menu_helpers.h"

#define BAG_WIN_ID_ITEMS         0
#define BAG_WIN_ID_MSG           1
#define BAG_WIN_ID_TITLE         2

// window indexes used in ShowBagWindow(index, nitems)
#define BAG_WIN_CHOOSE_QUANTITY   0
#define BAG_WIN_BUY_QUANTITY    1
#define BAG_WIN_MONEY           2
#define BAG_WIN_CHOOSE_QUANTITY2        3 // little box bottom right
#define BAG_WIN_4               4
#define BAG_WIN_MSG_W26         5 // fill MSG box
#define BAG_WIN_MSG_W14         6 // spare icon left
#define BAG_WIN_MSG_W15         7 // BAG_WIN_MSG_W14 + 1 width
#define BAG_WIN_MSG_W16         8 // BAG_WIN_MSG_W14 + 2 width
#define BAG_WIN_MSG_W23         9 // BAG_WIN_MSG_W14 and fill until right 
#define BAG_WIN_CONTEXT_H2      10
#define BAG_WIN_CONTEXT_H4      11
#define BAG_WIN_CONTEXT_H6      12
#define BAG_WIN_CONTEXT_H8      13


void InitBagWindows(void);
void BagPrintTextOnWindow(u8 windowId, u8 fontId, const u8 * str, u8 x, u8 y, u8 letterSpacing, u8 lineSpacing, u8 speed, u8 colorIdx);
void BagPrintTextOnWin1CenteredColor0(const u8 * str, u8 unused);
void BagDrawDepositItemTextBox(void);
u8 ShowBagWindow(u8 whichWindow, u8 nItems);
void HideBagWindow(u8 whichWindow);
u8 OpenBagWindow(u8 whichWindow);
void CloseBagWindow(u8 whichWindow);
u8 GetBagWindow(u8 whichWindow);
void BagCreateYesNoMenuBottomRight(u8 taskId, const struct YesNoFuncTable * ptrs);
void BagCreateYesNoMenuTopRight(u8 taskId, const struct YesNoFuncTable * ptrs);
void BagPrintMoneyAmount(void);
void BagDrawTextBoxOnWindow(u8 windowId);

#endif //GUARD_BAG_H
