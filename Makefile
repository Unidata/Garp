#
#
include $(CONFIGDIR)/Makeinc.common

SUBDIRS = build dcshef_comet

all :
	@for dir in $(SUBDIRS); do \
	     cd $$dir; echo -n "making in " ; \
	             pwd ; $(MAKE) $(MFLAGS) $@ ; cd .. ; \
	done;

install :
	@for dir in $(SUBDIRS); do \
	            cd $$dir; echo -n "installing in " ; \
	             pwd ; $(MAKE) $(MFLAGS) install ; cd ../ ; \
	done
	$(RM) $(GEMPAK)/tables/config/ntl.tbl; \
	$(CP) $(GEMPAK)/tables/config/garp.tbl $(GEMPAK)/tables/config/ntl.tbl;


clean:
	@for dir in $(SUBDIRS) ; do \
	      	    cd $$dir; echo -n "making clean in " ; \
	             pwd ; $(MAKE) $(MFLAGS) $@ ; cd ../ ; \
	done
	$(RM) $(GEMPAK)/tables/config/ntl.tbl; \
	$(CP) $(GEMPAK)/tables/config/ntl.tbl.orig $(GEMPAK)/tables/config/ntl.tbl;

