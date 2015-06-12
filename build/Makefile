
include $(GARPHOME)/config/Makeinc.common

SUBDIRS = \
	init/$(NA_OS)			\
	gui/$(NA_OS) 			\
	object/$(NA_OS)			\
	util/$(NA_OS)			\
	gempak/$(NA_OS)			\
	$(XPMSOURCE)			


LINKORDERGARPLIBS =  \
           $(GARPHOME)/init/$(NA_OS)/init.a 	\
           $(GARPHOME)/gui/$(NA_OS)/gui.a 	\
           $(GARPHOME)/object/$(NA_OS)/object.a \
           $(GARPHOME)/gempak/$(NA_OS)/gempak.a \
           $(GARPHOME)/util/$(NA_OS)/util.a 	\
           $(GARPHOME)/gempak/$(NA_OS)/gempak.a \
           $(GARPHOME)/gui/$(NA_OS)/gui.a 	\
	   $(XPMLIB)


GARPLIBS = \
           $(GARPHOME)/init/$(NA_OS)/init.a \
           $(GARPHOME)/gui/$(NA_OS)/gui.a \
           $(GARPHOME)/gempak/$(NA_OS)/gempak.a \
           $(GARPHOME)/object/$(NA_OS)/object.a \
           $(GARPHOME)/util/$(NA_OS)/util.a	\
	   $(XPMLIBARCH)



GARPAUX = \
	   garp/COPYRIGHT \
	   garp/README \
	   garp/config \
	   garp/fdf \
	   garp/html \
	   garp/icons/*.xbm \
	   garp/map \
	   garp/tables


NAGARPAUX = \
	   resource/Garp* \
	   icons/garp.xbm \
	   tables/ntl/ntl.tbl \
	   config/Makeinc.*


GARPSOURCE = \
	   garp/Makefile 		\
	   garp/gempak/Makefile  	\
	   garp/gempak/*.c  		\
	   garp/gempak/*.f 		\
	   garp/gui/Makefile  		\
	   garp/gui/*.c 		\
	   garp/init/Makefile  		\
	   garp/init/*.c 		\
	   garp/object/Makefile  	\
	   garp/object/*.c 		\
	   garp/util/Makefile  		\
	   garp/util/*.c 		\
	   garp/xpm/Makefile  		\
	   garp/xpm/COPYRIGHT  		\
	   garp/xpm/README		\
	   garp/xpm/*.c 		\
	   garp/xpm/*.h			\
	   garp/include


GARPEXE = \
	   exe/$(NA_OS)/garp



LIBS =   $(XDLIBS) $(XLIBS) $(OSLIB)


TARGET = $(NA_OS)/garp

all: startdate LIB_DIR_ALL $(TARGET) enddate

install: all
	@echo "Installing garp binary and resource files..."
	cp -f $(NA_OS)/garp $(OS_BIN)
	#strip $(OS_BIN)/garp
	cp -f config/Garp config/Garp2 $(NAWIPS)/resource
	

MAIN_OBJ     = init/$(NA_OS)/main.o
#SPECIAL_OBJS =  gempak/$(NA_OS)/ffdriv.o gempak/$(NA_OS)/imisub.o
SPECIAL_OBJS =  gempak/$(NA_OS)/imisub.o

startdate:
	@echo "Make start time is `date`"
enddate:
	@echo "Make end time is `date`"

$(TARGET): $(OBJ) $(GARPLIBS) $(SPECIAL_OBJS) $(NA_OS)
	@case $(NA_OS) in \
	linuxf2c ) \
	$(LINK.c) $(NOFORMAIN) \
	        -o $@ \
	        $(MAIN_OBJ) $(SPECIAL_OBJS) \
	        $(LINKORDERGARPLIBS) \
	        $(OS_LIB)/sfmap.a \
	        $(OS_LIB)/snmap.a \
	        $(OS_LIB)/snprof.a \
		$(OS_LIB)/sncross.a \
		$(OS_LIB)/gdcntr.a \
		$(OS_LIB)/gdthgt.a \
		$(OS_LIB)/gdprof.a \
		$(OS_LIB)/gdcross.a \
		$(OS_LIB)/gdmap.a \
		$(OS_LIB)/gdstream.a \
	        $(OS_LIB)/ginitp_alt.o \
	        $(OS_LIB)/gendp_alt.o \
		$(OS_LIB)/gemlib.a \
	        $(OS_LIB)/cgemlib.a \
	        $(OS_LIB)/gplt.a \
	        $(OS_LIB)/device.a \
	        $(OS_LIB)/Nxmlib.a \
	        $(OS_LIB)/xw.a \
	        $(OS_LIB)/gn.a \
	        $(OS_LIB)/cgemlib.a \
		$(OS_LIB)/gemlib.a \
		$(NLIB) \
		$(NETCDF) \
		$(LIBJASPER) \
		$(PNGLIB) \
        	$(XMLLIB) \
        	$(ZLIB) \
		$(RSLLIB) \
		$(LIBBZ2) \
	        $(MOTIFLIBS) \
	        $(SYSLIBS) \
	        $(XWLIBDIR) $(X11LIBS) -lXext -lXp \
		$(STATICSYSLIBS) /usr/local/lib/libf2c.a -lm ;; \
	* ) \
	$(LINK.f) $(NOFORMAIN) $(LDFLAGS) \
	        -o $@ \
	        $(MAIN_OBJ) $(SPECIAL_OBJS) \
	        $(LINKORDERGARPLIBS) \
	        $(OS_LIB)/sfmap.a \
	        $(OS_LIB)/snmap.a \
	        $(OS_LIB)/snprof.a \
		$(OS_LIB)/sncross.a \
		$(OS_LIB)/gdcntr.a \
		$(OS_LIB)/gdthgt.a \
		$(OS_LIB)/gdprof.a \
		$(OS_LIB)/gdcross.a \
		$(OS_LIB)/gdmap.a \
		$(OS_LIB)/gdstream.a \
	        $(GPLT_NC) \
	        $(OS_LIB)/Nxmlib.a \
		$(OS_LIB)/cgemlib.a \
	        $(OS_LIB)/gemlib.a \
	        $(OS_LIB)/gplt.a \
	        $(OS_LIB)/device.a \
	        $(OS_LIB)/xw.a \
	        $(OS_LIB)/gn.a \
	        $(OS_LIB)/gemlib.a \
		$(OS_LIB)/cgemlib.a \
		$(NLIB) \
		$(NETCDF) \
		$(LIBJASPER) \
		$(PNGLIB) \
                $(XMLLIB) \
                $(ZLIB) \
		$(RSLLIB) \
		$(LIBBZ2) \
	        $(GUILIBS) \
	        $(SYSLIBS) \
	        $(STATICSYSLIBS) -lm ;; \
        esac
	@echo "Make $(TARGET) end time is `date`"

$(NA_OS):
	@test ! -d $(NA_OS) && mkdir $(NA_OS)

reallyclean: clean cleansubdirs

clean: LIB_DIR_CLEAN
	@echo "Making clean in `pwd`" 
	rm -f $(TARGET)

cleansubdirs:
	@echo "Removing $(NA_OS) directories - they must be empty"
	-$(RMDIR) $(SUBDIRS) $(NA_OS) 

clobber: LIB_DIR_CLOBBER
	rm -f $(TARGET)

tarsrc:
	@echo "Making a GARP source distribution"
	cd $(GARPHOME)/.. ; tar cvf $(GARPHOME)/garpsrc.tar $(GARPSOURCE) $(GARPAUX)
	cd $(NAWIPS) ; tar rvf $(GARPHOME)/garpsrc.tar $(NAGARPAUX)
	compress $(GARPHOME)/garpsrc.tar

tarbin:
	@echo "Making a $(NA_OS) GARP binary distribution: garp.$(NA_OS).tar"
	cd $(GARPHOME)/.. ; tar cvf $(GARPHOME)/garp.$(NA_OS).tar $(GARPAUX)
	cd $(NAWIPS) ; tar rvf $(GARPHOME)/garp.$(NA_OS).tar $(NAGARPAUX) $(GARPEXE)
	compress $(GARPHOME)/garp.$(NA_OS).tar


n54comet:
	cd $(GARPHOME)/gempak ; rm -f gemimgcommon.f ; cp gemimgcommon.f-N54_COMET gemimgcommon.f

n54std:
	cd $(GARPHOME)/gempak ; rm -f gemimgcommon.f ; cp gemimgcommon.f-N54_STD gemimgcommon.f



LIB_DIR_ALL:
	@for dir in NODIR $(SUBDIRS); do \
		case $$dir in \
			NODIR) ;; \
			*) ( test ! -d $$dir && mkdir $$dir; \
				cd $$dir; echo "Making all in `pwd`" ; \
				$(MAKE) $(MFLAGS) -f ../Makefile all) ;; \
		esac ; \
	done


LIB_DIR_CLEAN:
	@for dir in NODIR $(SUBDIRS); do \
		case $$dir in \
			NODIR) ;; \
		*) (test ! -d $$dir && mkdir $$dir; \
				cd $$dir; echo "Making clean in `pwd`" ; \
				$(MAKE) $(MFLAGS) -f ../Makefile clean) ;; \
		esac ; \
	done

LIB_DIR_CLOBBER:
	@for dir in NODIR $(SUBDIRS); do \
		case $$dir in \
			NODIR) ;; \
		*) (test ! -d $$dir && mkdir $$dir; \
				cd $$dir; echo "Making clobber in `pwd`" ; \
				$(MAKE) $(MFLAGS) -f ../Makefile clobber) ;; \
		esac ; \
	done

help:
	@echo "make options:"
	@echo " "
	@echo "     all           builds program"
	@echo "     clean         cleans program, .o and .a files"
	@echo "     clobber       same as clean"
	@echo "     cleansubdirs  removes the OS subdirs"
	@echo "     install       builds and installs"
	@echo "     help          this message"
	@echo "     reallyclean   does clean and cleansubdirs"
	@echo "     tarbin        makes a binary-only, OS-specific tarfile"
	@echo "     tarsrc        makes a source tarfile"
	@echo " "
	@echo "  One of the following two options should be run BEFORE"
	@echo "  GARP is built via make"
	@echo " "
	@echo "     n54std        copies a file that prepares GARP to be built"
	@echo "                   with the standard version of NAWIPS5.4"
	@echo "     n54comet      copies a file that prepares GARP to be built"
	@echo "                   with the COMET version of NAWIPS5.4"      


