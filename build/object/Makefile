
include $(GARPHOME)/config/Makeinc.common

THIS_LIB = object.a

OBJ =	\
	animobj.o 	\
	bgimageobj.o	\
	datatimeobj.o	\
	gdinfobj.o	\
	globobj.o	\
        georefobj.o	\
	gridhobj.o	\
	gridxobj.o	\
	imageobj.o	\
	latlonobj.o	\
        mapobj.o	\
	mapbgobj.o	\
	markerobj.o	\
        mot.o		\
        objectmgr.o	\
        pot.o		\
        processdsp.o	\
	radarobj.o	\
	rringobj.o	\
	satobj.o	\
	surfobj.o	\
	thgtobj.o	\
	timemgmt.o	\
	titleobj.o	\
	upaobj.o	\
	upaprof.o	\
	updateobj.o	\
	updgrid.o	\
	updimage.o	\
	updsurf.o	\
	updupa.o	\
	updwprof.o	\
	utilobj.o	\
	vecthobj.o	\
	vprofobj.o	\
        winobj.o	\
	wprofobj.o	\
	displaybgimage.o	\
	displaygridh.o		\
	displaybgobj.o		\
	displaymarker.o		\
	displayradar.o		\
	displayrring.o		\
	displaysat.o		\
	displaysurf.o		\
	displaythgt.o		\
	displayupa.o		\
	displayupaprof.o	\
	displayvprof.o		\
	displayvectorh.o	\
	displaywindp.o		\
	displayxsec.o

all : $(THIS_LIB)


$(OBJ):
	$(COMPILE.c) ../$(@:.o=.c) \
		$(X11INC) \
		$(MOTIFINC) \
		$(GARPINCLUDE) \
		$(GEMINCDIR) \
		$(XPMINCLUDE)

$(THIS_LIB): $(OBJ)
	-@$(RM) $(THIS_LIB)
	@echo "Creating archive `pwd`/$(THIS_LIB)..."
	$(AR) $(ARFLAGS) $(THIS_LIB) $(OBJ)

clean :
	-$(RM) $(OBJ) $(THIS_LIB)

clobber:
	-$(RM) *.o *~* core $(THIS_LIB)

animobj.o: ../animobj.c
bgimageobj.o: ../bgimageobj.c
datatimeobj.o: ../datatimeobj.c
displaybgimage.o: ../displaybgimage.c
displaygridh.o: ../displaygridh.c
displaybgobj.o: ../displaybgobj.c
displaymarker.o: ../displaymarker.c
displayradar.o: ../displayradar.c
displayrring.o: ../displayrring.c
displaysat.o: ../displaysat.c
displaysurf.o: ../displaysurf.c
displaythgt.o: ../displaythgt.c
displayupa.o: ../displayupa.c
displayupaprof.o: ../displayupaprof.c
displayvectorh.o: ../displayvectorh.c
displayvprof.o: ../displayvprof.c
displaywindp.o: ../displaywindp.c
displayxsec.o: ../displayxsec.c
gdinfobj.o: ../gdinfobj.c
globobj.o: ../globobj.c
georefobj.o: ../georefobj.c
gridhobj.o: ../gridhobj.c
gridxobj.o: ../gridxobj.c
imageobj.o: ../imageobj.c
latlonobj.o: ../latlonobj.c
mapbgobj.o: ../mapbgobj.c
mapobj.o: ../mapobj.c
markerobj.o: ../markerobj.c
mot.o: ../mot.c
objectmgr.o: ../objectmgr.c
pot.o: ../pot.c
processdsp.o: ../processdsp.c
radarobj.o: ../radarobj.c
rringobj.o: ../rringobj.c
satobj.o: ../satobj.c
surfobj.o: ../surfobj.c
thgtobj.o: ../thgtobj.c
timemgmt.o: ../timemgmt.c
titleobj.o: ../titleobj.c
upaobj.o: ../upaobj.c
upaprof.o: ../upaprof.c
updateobj.o: ../updateobj.c
updgrid.o: ../updgrid.c
updimage.o: ../updimage.c
updsurf.o: ../updsurf.c
updupa.o: ../updupa.c
updwprof.o: ../updwprof.c
utilobj.o: ../utilobj.c
vecthobj.o: ../vecthobj.c
vprofobj.o: ../vprofobj.c
winobj.o: ../winobj.c
wprofobj.o: ../wprofobj.c
