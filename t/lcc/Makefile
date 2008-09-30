# Wedit Makefile for project last_fm_proto_test
SRCDIR=c:\projects\last.fm
CFLAGS=-I"c:\projects\lcc\include" -DDEBUG  -g2  -ansic
CC=$(LCCROOT)\bin\lcc.exe
LINKER=$(LCCROOT)\bin\lcclnk.exe
OBJS=\
	http.obj \
	last.fm.obj \
	md5.obj \
	neflib.obj \
	test.obj

LIBS=user32.lib ws2_32.lib ntdll.lib
EXE=last_fm_proto_test.exe

$(EXE):	$(OBJS) Makefile
	$(LINKER)  -subsystem console -entry _main -o $(SRCDIR)\lcc\last_fm_proto_test.exe $(OBJS) $(LIBS)

# Build http.c
HTTP_C=\
	$(SRCDIR)\u.h\
	$(SRCDIR)\neflib.h\

http.obj: $(HTTP_C) $(SRCDIR)\http.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\http.c

# Build last.fm.c
LAST_FM_C=\
	$(SRCDIR)\u.h\
	$(SRCDIR)\md5.h\
	$(SRCDIR)\http.h\

last.fm.obj: $(LAST_FM_C) $(SRCDIR)\last.fm.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\last.fm.c

# Build md5.c
MD5_C=\
	$(SRCDIR)\md5.h\

md5.obj: $(MD5_C) $(SRCDIR)\md5.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\md5.c

# Build neflib.c
NEFLIB_C=\

neflib.obj: $(NEFLIB_C) $(SRCDIR)\neflib.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\neflib.c

# Build test.c
TEST_C=\
	$(SRCDIR)\last.fm.h\

test.obj: $(TEST_C) $(SRCDIR)\test.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\test.c

link:
	$(LINKER)  -subsystem console -entry _main -o $(SRCDIR)\lcc\last_fm_proto_test.exe $(OBJS) $(LIBS)

clean:
	del $(OBJS) last_fm_proto_test.exe
