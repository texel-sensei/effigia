DIR_FONTS = ./Fonts
DIR_SRC = ./src
DIR_BIN = ./bin

SRC_C = $(wildcard ${DIR_FONTS}/*.c ${DIR_SRC}/*.c)
SRC_O = $(patsubst %.c,${DIR_BIN}/%.o,$(notdir ${SRC_C}))

TARGET = epd
#BIN_TARGET = ${DIR_BIN}/${TARGET}

CC = gcc

MSG = -g -O0 -Wall
#DEBUG = -D USE_DEBUG
DEBUG =
CFLAGS += $(MSG) $(DEBUG) -std=c99

LIB = -lwiringPi -lm

${TARGET}:${SRC_O}
	$(CC) $(CFLAGS) $(SRC_O) -o $@ $(LIB)

${DIR_BIN}/%.o : $(DIR_SRC)/%.c
	$(CC) $(CFLAGS) -c  $< -o $@ $(LIB)

${DIR_BIN}/%.o:$(DIR_FONTS)/%.c
	$(CC) $(CFLAGS) -c  $< -o $@

clean :
	rm $(DIR_BIN)/*.*
	rm $(TARGET)
