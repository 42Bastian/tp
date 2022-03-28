# makefile for TP (turbo packer)

CC = g++
CFLAGS = -O2 -fomit-frame-pointer -Wno-multichar
LDFLAGS = -s
RM = rm -f
ifeq ($(OSTYPE),cygwin)
EXT = .exe
else
EXT=#
endif

EXECS = tp$(EXT) untp$(EXT) wav2lsf$(EXT)

all: $(EXECS)

%.o : %.cpp global.h
	$(CC) -c $< $(CFLAGS)

tp$(EXT): tp.o loadsave.o
	$(CC) -o $@ $(LDFLAGS) $(CFLAGS) tp.o loadsave.o

untp$(EXT): untp.o loadsave.o
	$(CC) -o $@ $(LDFLAGS) $(CFLAGS) untp.o loadsave.o

wav2lsf$(EXT): wav2lsf.o sample.o loadsave.o
	$(CC) -o $@ $(LDFLAGS) $(CFLAGS) wav2lsf.o loadsave.o sample.o

clean	:
	$(RM) *.o
	$(RM) $(EXECS)
	$(RM) *.bak
