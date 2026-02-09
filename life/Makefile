CC        ?= gcc

# from https://stackoverflow.com/questions/154630/recommended-gcc-warning-options-for-c
CFLAGS    += -Wall -Wextra -Wformat=2 -Wswitch-default -Wcast-align \
	     -Wpointer-arith -Wbad-function-cast -Wstrict-prototypes -Winline \
	     -Wundef -Wnested-externs -Wcast-qual -Wshadow -Wwrite-strings \
	     -Wconversion -Wunreachable-code -Wstrict-aliasing=2 -fno-common \
	     -fstrict-aliasing -std=c99 -pedantic
DBG_FLAGS  = -O0 -pg -include stdbool.h
REL_FLAGS  = -O3

LOC       := $(shell readlink -f .)
PRJ_ROOT  := $(LOC)/..

SRC_DIR   := $(LOC)
SRCS      := $(SRC_DIR)/life.c
# DEPS      := ...

TGT_DIR   := $(PRJ_ROOT)/target

REL_DIR   := $(TGT_DIR)/release
DBG_DIR   := $(TGT_DIR)/debug


ifeq ($(DBG),true)
	CFLAGS  += $(DBG_FLAGS)
	OUT_DIR := $(DBG_DIR)
else
	CFLAGS  += $(REL_FLAGS)
	OUT_DIR := $(REL_DIR)
endif

BIN_DIR   := $(OUT_DIR)/bin
OBJ_DIR   := $(OUT_DIR)/obj
# DEP_DIR   := $(OUT_DIR)/dep

.PHONY: clean life part


life: $(BIN_DIR)/life


part: $(BIN_DIR)/partitioned_life


$(BIN_DIR)/%: $(SRC_DIR)/%.c | $(BIN_DIR)
	@echo "building binary $@ from $< ..."
	$(CC) $(CFLAGS) $< -o $@
	@echo "... $@ built."

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR) $(OBJ_DIR):
	@mkdir -p $@

clean:
	rm -rf $(TGT_DIR)
