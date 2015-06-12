
include $(GARPHOME)/config/Makeinc.common

THIS_LIB = gui.a

OBJ =	\
	animcontrol.o		\
	callbacks.o		\
	dropdown.o 		\
	enhancecb.o		\
	file_browser.o		\
	framecontrolcb.o	\
	garplogo.o		\
	gdinfocb.o		\
	geticon.o		\
	gifcb.o			\
	gridhcb.o		\
	gridhfdf.o		\
	gridtcb.o		\
	gridtfdf.o		\
	gridupfcb.o		\
	gridxcb.o		\
	gridxfdf.o		\
	gridvcb.o		\
	gridvfdf.o		\
	gui.o			\
	guiobj.o		\
	guiutil.o		\
	imagecb.o		\
	infopop.o		\
	latloncb.o		\
	list.o			\
	lineattr.o		\
	log.o			\
	maincb.o		\
	mapcb.o			\
	mod.o			\
	overlaycb.o		\
	radarcb.o		\
	scrolledw.o		\
	sgridcb.o		\
	statusbar.o		\
	surfcb.o		\
	thgtcb.o		\
	time_cb.o		\
	upacb.o			\
	upaprofcb.o		\
	updatecb.o		\
	vgridcb.o		\
	vprof.o			\
	winactions.o		\
	wincb.o			\
	wprof.o			\
	xsec.o


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
	-$(RM) $(OBJ) core $(THIS_LIB)

clobber:
	-$(RM) *.o *~* core
	-$(RM) $(THIS_LIB)

animcontrol.o: ../animcontrol.c
callbacks.o: ../callbacks.c
dropdown.o: ../dropdown.c
enhancecb.o: ../enhancecb.c
file_browser.o: ../file_browser.c
framecontrolcb.o: ../framecontrolcb.c
garplogo.o: ../garplogo.c
gdinfocb.o: ../gdinfocb.c
geticon.o: ../geticon.c
gifcb.o: ../gifcb.c
gridhcb.o: ../gridhcb.c
gridhfdf.o: ../gridhfdf.c
gridtcb.o: ../gridtcb.c
gridtfdf.o: ../gridtfdf.c
gridupfcb.o: ../gridupfcb.c
gridxcb.o: ../gridxcb.c
gridxfdf.o: ../gridxfdf.c
gridvcb.o: ../gridvcb.c
gridvfdf.o: ../gridvfdf.c
gui.o: ../gui.c
guiobj.o: ../guiobj.c
guiutil.o: ../guiutil.c
#help.o: ../help.c
imagecb.o: ../imagecb.c
infopop.o: ../infopop.c
latloncb.o: ../latloncb.c
lineattr.o: ../lineattr.c
list.o: ../list.c
log.o: ../log.c
maincb.o: ../maincb.c
mapcb.o: ../mapcb.c
mod.o: ../mod.c
overlaycb.o: ../overlaycb.c
radarcb.o: ../radarcb.c
scrolledw.o: ../scrolledw.c
sgridcb.o: ../sgridcb.c
statusbar.o: ../statusbar.c
surfcb.o: ../surfcb.c
thgtcb.o: ../thgtcb.c
time_cb.o: ../time_cb.c
upacb.o: ../upacb.c
upaprofcb.o: ../upaprofcb.c
updatecb.o: ../updatecb.c
vgridcb.o: ../vgridcb.c
vprof.o: ../vprof.c
winactions.o: ../winactions.c
wincb.o: ../wincb.c
wprof.o: ../wprof.c
xsec.o: ../xsec.c
