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

RC      = windres
RCFLAGS = -O coff

ICO     = assets/icon.ico
RCFILE  = app.rc
RES     = app.res

.PHONY: all clean lint

all: $(EXE)

$(EXE): $(OBJ) $(RES)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(RES) $(LDFLAGS)

$(RCFILE): $(ICO)
	@echo IDI_ICON1 ICON "$(ICO)" > $(RCFILE)

$(RES): $(RCFILE)
	$(RC) $(RCFLAGS) $(RCFILE) -o $(RES)

clean:
	@if exist "$(EXE)" del /q "$(EXE)"
	@for %%f in ($(OBJ)) do if exist "%%f" del /q "%%f"
	@if exist "$(RES)" del /q "$(RES)"
	@if exist "$(RCFILE)" del /q "$(RCFILE)"

lint:
	@for %%f in ($(SRC) $(HDR)) do if exist "%%f" astyle --style=java --pad-oper --suffix=none --max-code-length=180 "%%f"
