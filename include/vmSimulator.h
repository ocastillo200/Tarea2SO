#ifndef VM_SIMULATOR_H
#define VM_SIMULATOR_H

typedef struct
{
    int *frames;
    int size;
    int index;
} PageTable;

void initPageTable(PageTable *pt, int size);
int isPageInTable(PageTable *pt, int page);
void replacePage(PageTable *pt, int page);
int simulateFIFO(int *pages, int num_pages, int frame_size);

#endif