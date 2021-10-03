#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>

#include <msx.h>
#include <msx/gfx.h>

#include "msxtext.h"

/* WORKAREA */
#define MSX_CSRY 0xf3dc  // Cursor Y position
#define MSX_CSRX 0xf3dd  // Cursor X position

// Text data
extern unsigned char glyphdata[];
extern unsigned char textdata[];

void setCursorPosition(uint8_t x, uint8_t y)
{
#asm
    ld ix, 2
    add ix, sp

    ld h, (ix)
    ld l, (ix + 2)
    call 0x00c6
#endasm
}

void putChar(char letter)
{
#asm
    ld ix, 2
    add ix, sp

    ld a, (ix)
    call 0x00a2
#endasm
}

void putString(char *string)
{
    int index;
    for (index = 0; index < strlen(string); index++)
    {
        putChar(string[index]);
    }
}

void initGraphics()
{
    uint8_t *address;
    uint16_t i;

    // screen mode
    set_color(15, 4, 4);
    set_mode(2);
    //set_mangled_mode();

    // Copy tables
    //fill(0, 0, 0x0800 * 3);
    /*
    for (i = 0; i < 0x100; i++)
    {
        vpoke(0x1800 + i, i);
        vpoke(0x1900 + i, i);
        vpoke(0x1a00 + i, i);
    }

    for (i = 0; i < 0x800; i++)
    {
        vpoke(0x2000 + i, 0xf0);
        vpoke(0x2800 + i, 0xf0);
        vpoke(0x3000 + i, 0xf0);
    }*/

}

/**
 * メイン
 */
void main()
{
    int i;
    Command command;

    initGraphics();

    MTInitWindow(0, WINMODE_PLAIN, 1, 0, 30, 1, 0xa1);
    MTInitWindow(1, WINMODE_PLAIN, 2, 2, 20, 10, 0xf1);
    MTInitWindow(2, WINMODE_PLAIN, 10, 13, 20, 8, 0xf1);

    MTClear(0, 0xf1);
    MTClear(1, 0xf1);
    MTClear(2, 0xf1);

    command.command_type = COMMAND_TEXT;
    command.parameter.CommandPrintText.key = 0x002;
    MTStoreCommand(0,&command);

    command.parameter.CommandPrintText.key = 0x003;
    MTStoreCommand(1,&command);

    command.parameter.CommandPrintText.key = 0x001;
    MTStoreCommand(2,&command);

    MTExecCommand(0);
    MTExecCommand(1);
    MTExecCommand(2);


    while(1){};
}
