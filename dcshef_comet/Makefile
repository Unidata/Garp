#
#	Makefile for GEMPAK 6.0
#		Builds programs
#		Unidata 11/99 Chiz
#
include $(CONFIGDIR)/Makeinc.common

PROG = dcshef_comet

COBJS = \
	dcshef_comet.o

FOBJS = \
	dcshcd.o \
	shcomm.o \
	shdate.o \
	shdeca.o \
	shdecb.o \
	shdece.o \
	shdgrp.o \
	shdinc.o \
	shgfld.o \
	shgfos.o \
	shgrpt.o \
	shrndt.o \
	shrtim.o \
        shunit.o

LIBINC = \
	$(GEMLIB) \
	$(APPL) \
	$(SYSLIB) \
	$(BRIDGE) \
        $(XMLLIB) \
	$(CGEMLIB) \
	$(GEMLIB) \
	$(LIBJASPER) \
	$(PNGLIB) \
	$(ZLIB)

CFLAGS = $(COPT)

all : $(COBJS) $(FOBJS) $(PROG)

$(PROG) : $(COBJS) $(LOBJS) $(LIBINC) 
	$(LINK.f) $(NOFORMAIN) *.o $(LIBINC) -o $@  

programs_nc :
	@echo -n "Not a graphics program - no nc version needed"

programs_gf :
	@echo -n "Not a graphics program - no gf version needed"

clean:
	$(RM) $(PROG) *.o


install: $(PROG)
	$(INSTALL) $(PROG) $(OS_BIN)
	$(AUXINSTALL) $?.hlp $(GEMHLP)/hlp
	$(AUXINSTALL) shef_comet.pack $(GEMTBL)/pack
	$(AUXINSTALL) shefunits_comet.tbl $(GEMTBL)/stns
	$(AUXINSTALL) shef_ncdc.tbl $(GEMTBL)/stns

test: $(PROG)
	$(PROG) -v 10  -c 981119/1900 -b 240 -d dcshef.log -a 1000 \
	-p shef_comet.pack -s shef_ncdc.tbl YYMMDD_shefcomet.gem <  NMCRRAMT.1119.192302

