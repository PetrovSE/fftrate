DEBD   = debian
USRD   = usr


ifeq ($(DSTD),)
PLUGD  = $(USRD)/lib/$(shell uname -i)-linux-gnu/alsa-lib
else
PLUGD  = $(USRD)/lib/$(DSTD)/alsa-lib
endif


all:
	( cd ../; $(MAKE) libs tools apps )
	mkdir -p $(PLUGD)
	cp ../bin/*.so $(PLUGD)

purge: clean
	( cd ../; $(MAKE) purge )
	rm -rf $(USRD)

clean:
	( cd $(DEBD); find -mindepth 1 -maxdepth 1 -type d | xargs rm -rf )
	rm -rf $(DEBD)/tmp
	rm -rf $(DEBD)/*-utils
	rm -f $(DEBD)/*substvars
	rm -f $(DEBD)/*.log
	rm -f $(DEBD)/files
	rm -f build-stamp
