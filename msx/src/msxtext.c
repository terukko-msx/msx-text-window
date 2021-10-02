#include <stdio.h>
#include <stdlib.h>

#include <msx.h>
#include <msx/gfx.h>

#include "msxtext.h"

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

// Text data
extern unsigned char glyphdata[];
extern unsigned char textdata[];

WindowInfo windowInfo[MAX_WINDOW];
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
Status MTInitWindow(uint8_t wid, uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t bgcolor)
{
    if (wid < 0 || wid >= MAX_WINDOW)
    {
        return STATUS_INVALID_VALUE;
    }

    windowInfo[wid].x = x;
    windowInfo[wid].y = y;
    windowInfo[wid].w = w;
    windowInfo[wid].h = h;
    windowInfo[wid].bgcolor = bgcolor;
    windowInfo[wid].cx = 0;
    windowInfo[wid].cy = 0;
    return STATUS_OK;
}

Status MTClear(uint8_t wid, uint8_t color)
{
    uint8_t y;
    for (y = windowInfo[wid].y; y < windowInfo[wid].y + windowInfo[wid].h; y++)
    {
        fill(ADDR_COL_TBL(windowInfo[wid].x, y), color, windowInfo[wid].w * 8);
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
