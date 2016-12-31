
PROJS = fft_test ini_test

include ../lib/makedef.mk

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

purge: clean
	rm -rf $(BIND)
