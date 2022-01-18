CC ?= gcc

ifndef NOSTATIC
	CFLAGS = -static
else
	CFLAGS = 
endif
ifndef ASSERTS
  CFLAGS += -DNDEBUG
endif
ifdef DEBUG
  CFLAGS += -g
else
  OLEVEL ?= -O3
endif
ifdef GPROF
  CFLAGS += -pg
endif

BINARIES := soerominfo

.PHONY: all
all: $(BINARIES)

soerominfo: soerominfo.c
	$(CC) $(OLEVEL) $(CFLAGS) -o $@ $^ -lm

.PHONY: clean
clean:
ifeq ($(OS),Windows_NT) 
	del /F /Q /A:- soerominfo.exe
else
	rm -rf $(BINARIES)
endif
