#include <stdio.h>
#include <stdlib.h>
#include "status.h"

#ifndef _MSXTEXT_H
#define _MSXTEXT_H

#define MAX_WINDOW 5
#define MAX_COMMAND_QUEUE 10

// Command Type
#define COMMAND_NOP 0
#define COMMAND_TEXT 1
#define COMMAND_SETPOS 2
#define COMMAND_LINEFEED 3

// Command Parameter
typedef union {

    struct {
        uint16_t key;
        uint8_t color;
    } CommandPrintText;

    struct {
        uint8_t x;
        uint8_t y;
    } CommandSetCursorPosition;

    struct {
        uint8_t num;
    } CommandLineFeed;

} CommandParameter;

typedef struct {
    uint8_t command_type;
    CommandParameter parameter;
} Command;

// Window mode
#define WINMODE_HIDDEN      0x00
#define WINMODE_SHOW        0x01
#define WINMODE_PLAIN       0x00
#define WINMODE_BORDER      0x02

// WindowInfo
typedef struct
{
    uint8_t mode;
    uint8_t x;
    uint8_t y;
    uint8_t w;
    uint8_t h;
    uint8_t bgcolor;

    //
    uint8_t cx;
    uint8_t cy;

    // Command Queue
    Command queue[MAX_COMMAND_QUEUE];
    uint8_t queue_num;
} WindowInfo;


// Functions
Status MTInitWindow(uint8_t id, uint8_t mode, uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t bgcolor);
Status MTPrint(uint8_t wid, uint16_t key);
Status MTClear(uint8_t wid, uint8_t color);
Status MTClearCommand(uint8_t wid);
Status MTStoreCommand(uint8_t wid, Command *param);
Status MTExecCommand(uint8_t wid);
#endif