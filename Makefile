# makefile for TP (turbo packer)

ifeq ($(OSTYPE),cygwin)
CC = x86_64-w64-mingw32-g++
else
CC = g++
endif

CFLAGS = -O3 -fomit-frame-pointer -Wno-multichar
LDFLAGS =
RM = rm -f
ifeq ($(OSTYPE),cygwin)
EXT =.exe
else
EXT=#
endif

EXECS = tp$(EXT) untp$(EXT) wav2lsf$(EXT)

all: $(EXECS)

%.o: %.cpp
	$(CC) -c $(CFLAGS) -o $@ $<

tp$(EXT): tp.o loadsave.o global.h
	$(CC) -o $@ $(LDFLAGS) $(CFLAGS) tp.o loadsave.o

untp$(EXT): untp.o loadsave.o
	$(CC) -o $@ $(LDFLAGS) $(CFLAGS) untp.o loadsave.o

wav2lsf$(EXT): wav2lsf.o sample.o loadsave.o
	$(CC) -o $@ $(LDFLAGS) $(CFLAGS) wav2lsf.o loadsave.o sample.o

clean	:
	$(RM) *.o
	$(RM) $(EXECS)
	$(RM) *.bak
	$(RM) *.pck
	$(RM) *~

zip	: all
	@(cd ..;\
	$(RM) tp.zip;\
	zip tp.zip tp/loadsave.cc tp/untp.cc tp/tp.cc tp/global.h tp/makefile tp/*.S;)
	mv ../tp.zip .
	$(RM) tpdos.zip
	zip tpdos.zip tp.exe untp.exe
