include ../src/lib/makedef.mk

all:
	( cd ../src/lib; $(MAKE) -f $(MAKEF) )
	( cd ../src/tools; $(MAKE) -f $(MAKEF) )

clean:
	( cd ../src/lib; $(MAKE) -f $(MAKEF) clean )
	( cd ../src/tools; $(MAKE) -f $(MAKEF) clean )

purge: clean
	rm -rf ../bin
	rm -rf ../lib
