
include $(GARPHOME)/config/Makeinc.common



THIS_LIB = gempak.a
#
# local ffdriv.f no longer needed
#SPECIAL_FOBJS = ffdriv.o imisub.o
SPECIAL_FOBJS = imisub.o
SPECIAL_OBJS = $(SPECIAL_FOBJS)

COBJ = 	\
	garperr.o	\
	gemclear.o	\
	gemcolormgt.o	\
	gemcoordutil.o	\
        gemgeorefup.o	\
	gemgetgridtimes.o	\
	geminit.o	\
	gemresize.o	\
	getimage.o	\
	gfuncutil.o	\
	gpmap.o		\
	imgcolormod.o	\
	pixmapping.o	\
	pointerinfo.o	\
	ptitle.o	\
	readimgdata.o	\
	readimghdr.o	\
	readstn1.o	\
	readstn2.o	\
	renderimg.o	\
	setupgemwin.o	\
	upa_parms.o	\
	xsetpxmdata.o

FOBJ =			\
	colorbardraw.o	\
	dataarea.o	\
	erwmsg.o	\
	gdinfo.o	\
	gemimgcommon.o	\
	get_sao_files.o \
	get_upa_files.o \
	getnxtgrd.o	\
	graphdraw.o	\
	gridtime.o 	\
	gqsatm.o 	\
	gresize.o 	\
	moddate.o	\
	obscount.o	\
	pbgmap.o	\
	pllgrid.o	\
	pmap.o		\
	prring.o	\
	psgrid.o	\
	psnprof.o	\
	pstream.o	\
	psurf.o		\
	psym.o		\
	pthgt.o		\
	pupa.o		\
	pvgrid.o	\
	pvprof.o	\
	pwprof.o	\
	pxsec.o		\
	set_barb_color.o	\
	wprofbor.o	\
	wprofstm.o	\
	wprofwnd.o

all: $(THIS_LIB) $(SPECIAL_OBJS) cleanlinks

$(COBJ):
	$(COMPILE.c) ../$(@:.o=.c) \
		$(X11INC) \
		$(MOTIFINC) \
		$(GARPINCLUDE) \
		$(GEMINCDIR) \
		-I$(GEMPAK)/source/diaglib/dg \
		$(XPMINCLUDE)

$(FOBJ):
	$(COMPILE.f) $(INCPARM) ../$(@:.o=.f)

$(THIS_LIB): $(COBJ) $(FOBJ)
	-@$(RM) $(THIS_LIB)
	@echo "Creating archive `pwd`/$(THIS_LIB)..."
	$(AR) $(ARFLAGS) $(THIS_LIB) $(COBJ) $(FOBJ)


$(SPECIAL_FOBJS): 
	@echo "Creating standalone object file for $(SPECIAL_FOBJS)"
	$(COMPILE.f) $(INCPARM) ../$(@:.o=.f)


clean:
	-$(RM) $(COBJ) $(FOBJ) $(THIS_LIB) $(SPECIAL_OBJS) 

clobber :
	-$(RM) $(COBJ) $(FOBJ) $(THIS_LIB) $(SPECIAL_OBJS)

cleanlinks :
	-@$(RM)

#
#  C dependency lines.
#
garperr.o: ../garperr.c
geminit.o: ../geminit.c
gemclear.o: ../gemclear.c
gemcolormgt.o: ../gemcolormgt.c
gemcoordutil.o: ../gemcoordutil.c
gemgeorefup.o: ../gemgeorefup.c
gemgetgridtimes.o: ../gemgetgridtimes.c
gemresize.o: ../gemresize.c
getimage.o: ../getimage.c
gfuncutil.o: ../gfuncutil.c
gpmap.o: ../gpmap.c
imgcolormod.o: ../imgcolormod.c
pixmapping.o: ../pixmapping.c
ptitle.o: ../ptitle.c
readimgdata.o: ../readimgdata.c
readimghdr.o: ../readimghdr.c
readstn1.o: ../readstn1.c
readstn2.o: ../readstn2.c
renderimg.o: ../renderimg.c
setupgemwin.o: ../setupgemwin.c
xsatim.o: ../xsatim.c
xsetpxmdata.o: ../xsetpxmdata.c

#
# Fortran dependency lines
#
colorbardraw.o: ../colorbardraw.f
dataarea.o: ../dataarea.f
erwmsg.o: ../erwmsg.f
#ffdriv.o: ../ffdriv.f
gdinfo.o: ../gdinfo.f
gemimgcommon.o: ../gemimgcommon.f
get_sao_files.o: ../get_sao_files.f
get_upa_files.o: ../get_upa_files.f
getnxtgrd.o: ../getnxtgrd.f
graphdraw.o: ../graphdraw.f
gridclose.o: ../gridclose.f
gridopen.o: ../gridopen.f
gridtime.o: ../gridtime.f
dgfclos.o: ../dgfclos.f
gqsatm.o: ../gqsatm.f
gresize.o: ../gresize.f
imisub.o: ../imisub.f
moddate.o: ../moddate.f
obscount.o: ../obscount.f
pllgrid.o: ../pllgrid.f
pbgmap.o: ../pbgmap.f
pmap.o: ../pmap.f
prring.o: ../prring.f
psgrid.o: ../psgrid.f
psnprof.o: ../psnprof.f
pstream.o: ../pstream.f
psurf.o: ../psurf.f
psym.o: ../psym.f
pthgt.o: ../pthgt.f
pupa.o: ../pupa.f
pvgrid.o: ../pvgrid.f
pvprof.o: ../pvprof.f
pwprof.o: ../pwprof.f
pxsec.o: ../pxsec.f
pointerinfo.o: ../pointerinfo.c
set_barb_color.o: ../set_barb_color.f
snsbor.o: ../snsbor.f
snsstm.o: ../snsstm.f
snswnd.o: ../snswnd.f
