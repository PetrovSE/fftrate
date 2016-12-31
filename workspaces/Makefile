
include ../lib/makedef.mk

all:
	( cd ../lib; $(MAKE) -f $(MAKEF) )
	( cd ../apps; $(MAKE) )
	( cd ../tests; $(MAKE) )

clean:
	( cd ../lib; $(MAKE) -f $(MAKEF) clean )
	( cd ../apps; $(MAKE) clean )
	( cd ../tests; $(MAKE) clean )

purge:
	( cd ../lib; $(MAKE) -f $(MAKEF) purge )
	( cd ../apps; $(MAKE) purge )
	( cd ../tests; $(MAKE) purge )
