#ifndef GUARD_SUMMARY_PAGE
#define GUARD_SUMMARY_PAGE

#include "main.h"

typedef struct {
    u8 index;
    void (*CurrentTask)(u8 taskID);

} SummaryPage;

SummaryPage *Page_Info_Init(void)
{
    SummaryPage *page = AllocZeroed(sizeof(SummaryPage));
    page->index = 0;
    return page;
}

SummaryPage *Page_Skills_Init(void)
{
    SummaryPage *page = AllocZeroed(sizeof(SummaryPage));
    page->index = 1;
    return page;
}

SummaryPage *Page_Moves_Init(void)
{
    SummaryPage *page = AllocZeroed(sizeof(SummaryPage));
    page->index = 2;
    return page;
}

SummaryPage *Page_MoveInfos_Init(void)
{
    SummaryPage *page = AllocZeroed(sizeof(SummaryPage));
    page->index = 3;
    return page;
}



#endif