TARGET = psp-ftpd
APPOBJS= psp_main.o util.o loadutil.o ftpd.o ftp.o sutils.o psp_init.o psp_cfg.o psp_pg.o psp_font.o

OBJS = $(APPOBJS) 
LIBS=-lpspwlan -lpsppower -lpspumd -lpng -lz -lm

PSP_FW_VERSION=303
PSPFTPD_VERSION=0.4.1

INCDIR = 
CFLAGS = -O2 -Wall -DPSPFW30X \
 -funroll-loops -ffast-math -fomit-frame-pointer \
 -fno-strength-reduce -finline-functions \
 -fsingle-precision-constant

ASFLAGS = $(CFLAGS) -c

LIBDIR =
LDFLAGS =
BUILD_PRX=1

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = PSP-FTPD-v$(PSPFTPD_VERSION)
PSP_EBOOT_ICON= pspftpd.png
PSP_EBOOT_PIC1= pspftpd-pic.png
# PSP_EBOOT_SND0= pspftpd.at3

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

all: EBOOT.PBP

install: all
	@mkdir -p /media/disk/psp/game4XX/psp-ftpd/
	cp EBOOT.PBP /media/disk/psp/game4XX/psp-ftpd/

realclean:
	/bin/rm -f $(OBJS) EBOOT.PBP PARAM.SFO


