
include lib/makedef.mk

all:
	( cd lib; $(MAKE) -f $(MAKEF) )
	( cd apps; $(MAKE) -f $(MAKEF) )
	( cd tests; $(MAKE) -f $(MAKEF) )
	( cd tools; $(MAKE) -f $(MAKEF) )

clean:
	( cd lib; $(MAKE) -f $(MAKEF) clean )
	( cd apps; $(MAKE) -f $(MAKEF) clean )
	( cd tests; $(MAKE) -f $(MAKEF) clean )
	( cd tools; $(MAKE) -f $(MAKEF) clean )

	rm -rf build-*-*-


purge: clean
	rm -rf bin
