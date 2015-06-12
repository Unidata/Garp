
include $(GARPHOME)/config/Makeinc.common

THIS_LIB = init.a

OBJ =	\
	init_actions.o		\
	garpinit.o		\
	config.o		\
	dialogs.o		\
	init_data.o		\
	init_tables.o		\
	init_time.o

MAIN_OBJ = main.o

all : $(THIS_LIB) $(MAIN_OBJ)


$(OBJ):
	$(COMPILE.c) ../$(@:.o=.c) \
		$(X11INC) \
		$(MOTIFINC) \
		$(GARPINCLUDE) \
		$(GEMINCDIR) \
		$(XPMINCLUDE)

$(MAIN_OBJ):
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
	-$(RM) $(OBJ) $(THIS_LIB) $(MAIN_OBJ)

clobber:
	-$(RM) *.o *~* core $(THIS_LIB) $(MAIN_OBJ)

main.o: ../main.c
garpinit.o: ../garpinit.c
config.o: ../config.c
dialogs.o: ../dialogs.c
init_actions.o: ../init_actions.c
init_data.o: ../init_data.c
init_tables.o: ../init_tables.c
init_time.o: ../init_time.c
