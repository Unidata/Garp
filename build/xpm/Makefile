
include $(GARPHOME)/config/Makeinc.common

THIS_LIB = xpm.a

OBJ= 	\
	data.o 			\
	create.o 		\
	hashtable.o		\
	misc.o			\
	rgb.o			\
	scan.o			\
	parse.o			\
	XpmCrBufFrI.o		\
	XpmCrBufFrP.o		\
	XpmCrDataFrI.o		\
	XpmCrDataFrP.o		\
	XpmCrIFrBuf.o		\
	XpmCrIFrData.o		\
	XpmCrPFrBuf.o		\
	XpmCrPFrData.o		\
	XpmRdFToData.o		\
	XpmRdFToI.o		\
	XpmRdFToP.o		\
	XpmWrFFrData.o		\
	XpmWrFFrI.o		\
	XpmWrFFrP.o

all : $(THIS_LIB)


$(OBJ):
	$(COMPILE.c) $(XPMDEFINES) ../$(@:.o=.c) \
		$(X11INC) \
		$(MOTIFINC) \
		$(XPMINCLUDE)

$(THIS_LIB): $(OBJ)
	-@$(RM) $(THIS_LIB)
	@echo "Creating archive `pwd`/$(THIS_LIB)..."
	$(AR) $(ARFLAGS) $(THIS_LIB) $(OBJ)

clean :
	-$(RM) $(OBJ) core $(THIS_LIB)

clobber:
	-$(RM) *.o *~* core $(THIS_LIB)


data.o: ../data.c
create.o: ../create.c
hashtable.o: ../hashtable.c
misc.o: ../misc.c
rgb.o: ../rgb.c
scan.o: ../scan.c
parse.o: ../parse.c
XpmCrBufFrI.o: ../XpmCrBufFrI.c
XpmCrBufFrP.o: ../XpmCrBufFrP.c
XpmCrDataFrI.o: ../XpmCrDataFrI.c
XpmCrDataFrP.o: ../XpmCrDataFrP.c
XpmCrIFrBuf.o: ../XpmCrIFrBuf.c
XpmCrIFrData.o: ../XpmCrIFrData.c
XpmCrPFrBuf.o: ../XpmCrPFrBuf.c
XpmCrPFrData.o: ../XpmCrPFrData.c
XpmRdFToData.o: ../XpmRdFToData.c
XpmRdFToI.o: ../XpmRdFToI.c
XpmRdFToP.o: ../XpmRdFToP.c
XpmWrFFrData.o: ../XpmWrFFrData.c
XpmWrFFrI.o: ../XpmWrFFrI.c
XpmWrFFrP.o: ../XpmWrFFrP.c

