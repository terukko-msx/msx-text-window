CC            = zcc
CFLAGS        = +msx -subtype=rom -create-app
LIBS          = -lmsxbios
SRCS          = msx/src/main.c msx/src/msxtext.c msx/src/data.asm
PROGRAM       = textwin.rom

all:            $(PROGRAM)

$(PROGRAM): $(OBJS)
	$(CC) $(CFLAGS) $(LIBS) -o $(PROGRAM) $(SRCS)

clean:;         rm -f $(PROGRAM)
