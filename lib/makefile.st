PROJS = cmdline convert fft mathex profiler riffio thr

include makedef.mk

all:
	for proj in $(PROJS);					\
	do							\
		( cd $$proj; $(MAKE) -f $(MAKEF) );		\
	done

clean:
	for proj in $(PROJS);					\
	do							\
		( cd $$proj; $(MAKE) -f $(MAKEF) clean );	\
	done

	rm -rf release
	rm -rf debug

	rm -f Makefile
	rm -f Makefile.*

purge: clean
	rm -rf $(BIND)
