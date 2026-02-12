CC         = gcc

# from https://stackoverflow.com/questions/154630/recommended-gcc-warning-options-for-c
CFLAGS    += -Wall -Wextra -Wformat=2 -Wswitch-default -Wcast-align \
	     -Wpointer-arith -Wbad-function-cast -Wstrict-prototypes -Winline \
	     -Wundef -Wnested-externs -Wcast-qual -Wshadow -Wwrite-strings \
	     -Wconversion -Wunreachable-code -Wstrict-aliasing=2 -fno-common \
	     -fstrict-aliasing -std=c99 -pedantic
	     
DBG_FLAGS  = -O0 -g -include stdbool.h
REL_FLAGS  = -O3
PRF_FLAGS  = -pg

LOC       := $(shell readlink -f .)
PRJ_ROOT  := $(LOC)/..

TGT_DIR   := $(PRJ_ROOT)/target/life

REL_DIR   := $(TGT_DIR)/release
DBG_DIR   := $(TGT_DIR)/debug


ifeq ($(DBG),true)
	CFLAGS  += $(DBG_FLAGS)
	OUT_DIR := $(DBG_DIR)
else ifeq ($(PRF),true)
	CFLAGS  += $(PRF_FLAGS)
	OUT_DIR := $(DBG_DIR)
else
	CFLAGS  += $(REL_FLAGS)
	OUT_DIR := $(REL_DIR)
endif

BIN_DIR   := $(OUT_DIR)/bin
SRC_DIR   := $(LOC)

.PHONY: clean life


life: $(BIN_DIR)/life


$(BIN_DIR)/life: $(SRC_DIR)/life.c $(OBJS) | $(BIN_DIR)
	@echo "building binary $@ from $< ..."
	$(CC) $(CFLAGS) $(CMPT_FLAGS) $< $(OBJS) -o $@
	@echo "... $@ built."

$(BIN_DIR):
	@mkdir -p $@

clean:
	rm -rf $(TGT_DIR)
