
include src/lib/makedef.mk

all: libs apps tools
	( cd src/tests; $(MAKE) -f $(MAKEF) )

libs:
	( cd src/lib; $(MAKE) -f $(MAKEF) )

apps:
	( cd src/apps; $(MAKE) -f $(MAKEF) )

tools:
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
