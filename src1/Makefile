# プログラム名とオブジェクトファイル名
PROJECT = example
TARGET = $(PROJECT).rom
SRC_DIR = src
OBJ_DIR = obj

OBJS = src/main.o src/msxtext.o src/data.o

# 定義済みマクロの再定義
CC = zcc
CFLAGS = +msx -debug -g3 -O0
TARGET_OPTION = +msx -subtype=rom -create-app -lmsxbios -debug -g3 -O0

# サフィックスルール適用対象の拡張子の定義
.SUFFIXES: .c .o
.SUFFIXES: .asm .o

all:	$(TARGET)

# プライマリターゲット
$(TARGET): $(OBJS)
	$(CC) $(TARGET_OPTION) -o $(TARGET) $^

# サフィックスルール
.c.o:
	$(CC) $(CFLAGS) -c $<

.asm.o:
	$(CC) $(CFLAGS) -c $<

# ファイル削除用ターゲット
.PHONY: clean
clean:
	$(RM) $(TARGET) $(OBJS) *.bin 

src/main.o:	src/main.c src/msxtext.h
src/msxtext.o:	src/msxtext.c src/msxtext.h src/status.h
 
