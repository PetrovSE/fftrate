
include lib/makedef.mk

all:
	( cd lib; $(MAKE) -f $(MAKEF) )
	( cd apps; $(MAKE) -f $(MAKEF) )
	( cd tests; $(MAKE) -f $(MAKEF) )

clean:
	( cd lib; $(MAKE) -f $(MAKEF) clean )
	( cd apps; $(MAKE) -f $(MAKEF) clean )
	( cd tests; $(MAKE) -f $(MAKEF) clean )

	rm -rf build-*-*-
	rm -f workspaces/*.user


purge: clean
	( cd lib; $(MAKE) -f $(MAKEF) purge )
	( cd apps; $(MAKE) -f $(MAKEF) purge )
	( cd tests; $(MAKE) -f $(MAKEF) purge )
