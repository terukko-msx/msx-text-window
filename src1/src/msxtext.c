#include <stdio.h>
#include <stdlib.h>

#include <msx.h>
#include <msx/gfx.h>

#include "msxtext.h"


// log
char log[255];


//
typedef struct
{
    uint8_t *text;
    uint8_t length
} TextInfo;

// TABLE ADDRESS
#define PATTERN_TBL 0x0000
#define NAME_TBL 0x1800
#define COLOR_TBL 0x2000

#define ADDR_PTN_TBL(x, y) ((y * 256) + (x * 8))
#define ADDR_NAM_TBL(x, y) (NAME_TBL + (y * 32) + x)
#define ADDR_COL_TBL(x, y) (COLOR_TBL + (y * 256) + (x * 8))

// ERROR Check
#define ASSERT_WINID(wid) (wid < 0 || wid >= MAX_WINDOW)

// Text data
extern unsigned char glyphdata[];
extern unsigned char textdata[];
extern uint8_t border_h[];
extern uint8_t border_v[];
extern uint8_t border_tl[];
extern uint8_t border_tr[];
extern uint8_t border_bl[];
extern uint8_t border_br[];

// Window Info Table
WindowInfo windowInfo[MAX_WINDOW];

// Text Parameter (Return Value)
TextInfo textinfo;

// Local
Status mtGetText(uint16_t key)
{
    uint8_t *pt = textdata;
    while (1)
    {
        if (*(uint16_t *)pt == 0xffff)
        {
            textinfo.length = 0;
            textinfo.text = NULL;
            break;
        }

        if (key == *(uint16_t *)pt)
        {
            textinfo.length = *(pt + 2);
            pt += 3;
            textinfo.text = pt;
            break;
        }
        else
        {
            pt = pt + 3 + *(pt + 2);
        }
    }
    return STATUS_OK;
}

// Public
Status MTInitWindow(uint8_t wid, uint8_t mode, uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t bgcolor)
{
    uint8_t index;

    if (wid < 0 || wid >= MAX_WINDOW)
    {
        return STATUS_INVALID_VALUE;
    }

    windowInfo[wid].mode = mode;
    windowInfo[wid].x = x;
    windowInfo[wid].y = y;
    windowInfo[wid].w = w;
    windowInfo[wid].h = h;
    windowInfo[wid].bgcolor = bgcolor;
    windowInfo[wid].cx = 0;
    windowInfo[wid].cy = 0;
    windowInfo[wid].queue_num = 0;

    for (index = 0; index < MAX_COMMAND_QUEUE; index++)
    {
        windowInfo[wid].queue[index].command_type = COMMAND_NOP;
    }

    return STATUS_OK;
}

bool mtIsInsideOfWindow(uint8_t wid, uint8_t x, uint8_t y)
{
    if (windowInfo[wid].w == 0 || windowInfo[wid].h == 0) {
        return false;
    }

    if (x >= windowInfo[wid].x && x <= (windowInfo[wid].x + windowInfo[wid].w - 1))
    {
        if (y >= windowInfo[wid].y && y <= (windowInfo[wid].y + windowInfo[wid].h - 1))
        {
            sprintf(log,"in(%d)[%d,%d](%d,%d,%d,%d)",wid,x,y,windowInfo[wid].x,windowInfo[wid].y,windowInfo[wid].w,windowInfo[wid].h);
            return true;
        }
    }
    sprintf(log,"out(%d)[%d,%d](%d,%d,%d,%d)",wid,x,y,windowInfo[wid].x,windowInfo[wid].y,windowInfo[wid].w,windowInfo[wid].h);
    return false;
}

uint8_t mtGetExposedWindow(uint8_t x, uint8_t y)
{
    uint8_t wid;
    for (wid = MAX_WINDOW - 1; wid >= 0; wid--)
    {
        if ((windowInfo[wid].mode & WINMODE_SHOW) == WINMODE_SHOW) {
            if (mtIsInsideOfWindow(wid, x, y)==true) {
                return wid;
            }
        }
        if (wid==0) break;
    }
    return NULL_WINDOW;
}

Status mtDrawBorder(uint8_t wid){
    uint8_t x, y, i;

    if ((windowInfo[wid].mode & WINMODE_BORDER) == 0) {
        return STATUS_OK;
    }

    x = windowInfo[wid].x;
    y = windowInfo[wid].y;

    if (mtGetExposedWindow(x,y)==wid) {
        vwrite(border_tl,ADDR_PTN_TBL(x,y),8);
        fill(ADDR_COL_TBL(x,y), 0xf1, 8);
    }

    x++;
    for (i=0;i<windowInfo[wid].w-2;i++){
        if (mtGetExposedWindow(x,y)==wid) {
            vwrite(border_h,ADDR_PTN_TBL(x,y),8);
            fill(ADDR_COL_TBL(x,y), 0xf1, 8);
        }
        x++;
    }

    if (mtGetExposedWindow(x,y)==wid) {
        vwrite(border_tr,ADDR_PTN_TBL(x,y),8);
        fill(ADDR_COL_TBL(x,y), 0xf1, 8);
    }

    x = windowInfo[wid].x;
    y = windowInfo[wid].y+1;
    for(i=0;i<windowInfo[wid].h-2;i++) {
        if (mtGetExposedWindow(x,y)==wid) {
            vwrite(border_v,ADDR_PTN_TBL(x,y),8);
            fill(ADDR_COL_TBL(x,y), 0xf1, 8);
        }
        if (mtGetExposedWindow(x+windowInfo[wid].w-1,y)==wid) {
            vwrite(border_v,ADDR_PTN_TBL((x+windowInfo[wid].w-1),y),8);
            fill(ADDR_COL_TBL((x+windowInfo[wid].w-1),y), 0xf1, 8);
        }
        y++;
    }

    x = windowInfo[wid].x;
    y = windowInfo[wid].y + windowInfo[wid].h -1;

    if (mtGetExposedWindow(x,y)==wid) {
        vwrite(border_bl,ADDR_PTN_TBL(x,y),8);
        fill(ADDR_COL_TBL(x,y), 0xf1, 8);
    }

    x++;
    for (i=0;i<windowInfo[wid].w-2;i++){
        if (mtGetExposedWindow(x,y)==wid) {
            vwrite(border_h,ADDR_PTN_TBL(x,y),8);
            fill(ADDR_COL_TBL(x,y), 0xf1, 8);
        }
        x++;
    }

    if (mtGetExposedWindow(x,y)==wid) {
        vwrite(border_br,ADDR_PTN_TBL(x,y),8);
        fill(ADDR_COL_TBL(x,y), 0xf1, 8);
    }

    return STATUS_OK;
}

Status MTClear(uint8_t wid, uint8_t color)
{
    uint8_t y;
    for (y = windowInfo[wid].y; y < windowInfo[wid].y + windowInfo[wid].h; y++)
    {
        fill(ADDR_COL_TBL(windowInfo[wid].x, y), color, windowInfo[wid].w * 8);
    }

    return mtDrawBorder(wid);
}

Status MTClearCommand(uint8_t wid)
{
    uint8_t index;
    for (index = 0; index < MAX_COMMAND_QUEUE; index++)
    {
        windowInfo[wid].queue[index].command_type = COMMAND_NOP;
    }
    windowInfo[wid].queue_num = 0;
}

Status MTStoreCommand(uint8_t wid, Command *param)
{
    if (ASSERT_WINID(wid))
        return STATUS_INVALID_VALUE;
    if (windowInfo[wid].queue_num == MAX_COMMAND_QUEUE)
        return STATUS_FULL;

    windowInfo[wid].queue[windowInfo[wid].queue_num] = *param;
    windowInfo[wid].queue_num++;
}

Status MTExecCommand(uint8_t wid)
{
    uint8_t index;
    if (ASSERT_WINID(wid))
        return STATUS_INVALID_VALUE;
    for (index = 0; index < windowInfo[wid].queue_num; index++)
    {
        switch (windowInfo[wid].queue[index].command_type)
        {
        case COMMAND_TEXT:
            MTPrint(wid, windowInfo[wid].queue[index].parameter.CommandPrintText.key);
            break;
        default:
            break;
        }
    }
}

Status MTPrint(uint8_t wid, uint16_t key)
{
    uint8_t status;
    uint8_t index;
    uint8_t x, y, max_x, max_y;
    uint8_t *glyph;

    status = mtGetText(key);
    if (status != STATUS_OK)
    {
        return status;
    }
    else
    {
        x = windowInfo[wid].x + windowInfo[wid].cx;
        y = windowInfo[wid].y + windowInfo[wid].cy;
        max_x = windowInfo[wid].x + windowInfo[wid].w - 1;
        max_y = windowInfo[wid].y + windowInfo[wid].h - 1;
        for (index = 0; index < textinfo.length; index++)
        {
            glyph = &glyphdata[textinfo.text[index] * 8];
            vwrite(glyph, ADDR_PTN_TBL(x, y), 8);
            fill(ADDR_COL_TBL(x, y), windowInfo[wid].bgcolor, 8);
            if (x == max_x)
            {
                if (y == max_y)
                {
                    return STATUS_OK;
                }
                x = windowInfo[wid].x;
                y++;
            }
            else
            {
                x++;
            }
        }
    }
    return STATUS_OK;
}

void initLog() {
    sprintf(log, "Log:");
}

void printLog(void) {
    set_mode(1);
    printf(log);
}