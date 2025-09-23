SHELL = cmd
.SHELLFLAGS = /C

CC = gcc
CFLAGS = -Wall -Wextra -Werror -Iinclude
LDFLAGS = -luser32 -lwinmm

SRC = src/main.c src/action.c src/utils.c src/parse.c src/task.c
OBJ = $(subst /,\,$(SRC:.c=.o))
HDR = $(wildcard include/*.h)

TARGET  = autostroke
EXE     = $(TARGET).exe

.PHONY: all clean

all: $(EXE)

$(EXE): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LDFLAGS)

clean:
	@if exist "$(EXE)" del /q "$(EXE)"
	@for %%f in ($(OBJ)) do if exist "%%f" del /q "%%f"

lint:
	@for %%f in ($(SRC) $(HDR)) do if exist "%%f" astyle --style=java --pad-oper --suffix=none --max-code-length=180 "%%f"
