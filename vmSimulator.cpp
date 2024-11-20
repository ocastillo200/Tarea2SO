#include <stdio.h>
#include <stdlib.h>
#include "include/vmSimulator.h"

void initPageTable(PageTable *pt, int size)
{
    pt->frames = (int *)malloc(size * sizeof(int));
    for (int i = 0; i < size; i++)
    {
        pt->frames[i] = -1;
    }
    pt->size = size;
    pt->index = 0;
}

int isPageInTable(PageTable *pt, int page)
{
    for (int i = 0; i < pt->size; i++)
    {
        if (pt->frames[i] == page)
        {
            return 1;
        }
    }
    return 0;
}

void replacePage(PageTable *pt, int page)
{
    pt->frames[pt->index] = page;
    pt->index = (pt->index + 1) % pt->size;
}

int simulateFIFO(int *pages, int num_pages, int frame_size)
{
    PageTable pt;
    initPageTable(&pt, frame_size);
    int page_faults = 0;

    for (int i = 0; i < num_pages; i++)
    {
        if (!isPageInTable(&pt, pages[i]))
        {
            replacePage(&pt, pages[i]);
            page_faults++;
        }
    }

    free(pt.frames);
    return page_faults;
}
