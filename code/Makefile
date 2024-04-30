NAME=radio
SDCC=sdcc

export PATH := /home/fademike/Programs/STMicroelectronics/stm8flash/:$(PATH)
CCFLAGS=-DSTM8S003 -I../ -I/usr/share/sdcc/include -mstm8 --out-fmt-ihx
LDFLAGS= -mstm8 --out-fmt-ihx -lstm8
FLASHFLAGS=-c stlinkv2 -p stm8s003f3

SRC=$(wildcard *.c)

OBJ=$(SRC:%.c=%.rel)
TRASH=$(OBJ) $(SRC:%.c=%.rst) $(SRC:%.c=%.asm) $(SRC:%.c=%.lst)
TRASH+=$(SRC:%.c=%.sym) $(NAME).lk $(NAME).map $(NAME).cdb
TRASH+=$(NAME).ihx
INDEPENDENT_HEADERS=../stm8l.h ../stm8s.h ports_definition.h Makefile

all: $(NAME).ihx

#$(SRC) : %.c : %.h $(INDEPENDENT_HEADERS)
#	@touch $@
#
#%.h: ;

clean:
	rm -f $(TRASH)

load: $(NAME).ihx
	stm8flash $(FLASHFLAGS) -w $(NAME).ihx

%.rel: %.c
	$(SDCC) $(CCFLAGS) -c $<

$(NAME).ihx: $(OBJ)
	$(SDCC) $(LDFLAGS) $(OBJ) -o $(NAME).ihx

.PHONY: all
