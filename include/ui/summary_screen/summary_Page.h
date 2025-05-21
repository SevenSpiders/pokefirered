#ifndef GUARD_SUMMARY_PAGE
#define GUARD_SUMMARY_PAGE

#include "main.h"
#include "malloc.h"

typedef struct {
    u8 index;
    u8 windowIds[4];
    void (*CurrentTask)(u8 taskID);

} SummaryPage;

// SummaryPage *Page_Info_Init(void)
// {
//     SummaryPage *page = AllocZeroed(sizeof(SummaryPage));
//     page->index = 0;
//     return page;
// }

// SummaryPage *Page_Skills_Init(void)
// {
//     SummaryPage *page = AllocZeroed(sizeof(SummaryPage));
//     page->index = 1;
//     return page;
// }

// SummaryPage *Page_Moves_Init(void)
// {
//     SummaryPage *page = AllocZeroed(sizeof(SummaryPage));
//     page->index = 2;
//     return page;
// }

// SummaryPage *Page_MoveInfos_Init(void);

// #include "summary_Page_MoveInfo.h"

#endif