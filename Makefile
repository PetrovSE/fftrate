
include src/lib/makedef.mk

all:
	( cd src/lib; $(MAKE) -f $(MAKEF) )
	( cd src/apps; $(MAKE) -f $(MAKEF) )
	( cd src/tests; $(MAKE) -f $(MAKEF) )
	( cd src/tools; $(MAKE) -f $(MAKEF) )

clean:
	( cd src/lib; $(MAKE) -f $(MAKEF) clean )
	( cd src/apps; $(MAKE) -f $(MAKEF) clean )
	( cd src/tests; $(MAKE) -f $(MAKEF) clean )
	( cd src/tools; $(MAKE) -f $(MAKEF) clean )

	rm -rf build-*-*-*


purge: clean
	rm -rf bin
	rm -rf lib
