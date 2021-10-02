#include <stdio.h>
#include <stdlib.h>
#include "status.h"

#ifndef _MSXTEXT_H
#define _MSXTEXT_H

#define MAX_WINDOW 5

typedef struct
{
    uint8_t x;
    uint8_t y;
    uint8_t w;
    uint8_t h;
    uint8_t bgcolor;

    //
    uint8_t cx;
    uint8_t cy;
} WindowInfo;

Status MTInitWindow(uint8_t id, uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t bgcolor);
Status MTPrint(uint8_t wid, uint16_t key);
Status MTClear(uint8_t wid, uint8_t color);

#endif