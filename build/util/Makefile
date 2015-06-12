
include $(GARPHOME)/config/Makeinc.common

THIS_LIB = util.a

OBJ =	\
	abortproc.o		\
	configfilemgmt.o	\
	depictable.o		\
	directoryscan.o		\
	fdf.o			\
	filecheck.o		\
	get_directory_list.o 	\
	get_table.o		\
	gifread.o		\
	gprint.o		\
	init_scrolledw.o	\
	math.o			\
	motifutil.o		\
	processfdf.o		\
	readlist.o		\
	readtbl.o		\
	stringclean.o		\
	stringlist.o		\
	stringtools.o		\
	template.o		\
	timestamp.o		\
	timeutil.o		\
	widgetutil.o		\
	xvgifwr.o		\
	Free.o

#	scandir.o

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
	-$(RM) *.o *~* core $(THIS_LIB)

abortproc.o: ../abortproc.c
configfilemgmt.o: ../configfilemgmt.c
depictable.o: ../depictable.c
directoryscan.o: ../directoryscan.c
fdf.o: ../fdf.c
filecheck.o: ../filecheck.c
get_directory_list.o: ../get_directory_list.c
get_table.o: ../get_table.c
gifread.o: ../gifread.c
gprint.o: ../gprint.c
init_scrolledw.o: ../init_scrolledw.c
math.o: ../math.c
motifutil.o: ../motifutil.c
processfdf.o: ../processfdf.c
readlist.o: ../readlist.c
readtbl.o: ../readtbl.c
scandir.o: ../scandir.c
stringclean.o: ../stringclean.c
stringlist.o: ../stringlist.c
stringtools.o: ../stringtools.c
template.o: ../template.c
timestamp.o: ../timestamp.c
timeutil.o: ../timeutil.c
widgetutil.o: ../widgetutil.c
xvgifwr.o: ../xvgifwr.c
Free.o: ../Free.c
