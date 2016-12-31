PLUG   = libasound_module_rate_fftrate
ARATE  = arateconf
MIXERC = amixrestore


PFILES = plugin.o
AFILES = main.o conf.o alsa.o


CONF   = fftrate.conf
ETCD   = etc
COMPTD = usr/lib/alsa-lib
BIND   = usr/bin
PROJD  = projects

PSRCD  = src
ASRCD  = $(PSRCD)/arateconf
MSRCD  = $(PSRCD)/script

INC    = -I ../lib
OBJD   = obj
LIBD   = ../lib/bin
DEBD   = debian

ifeq ($(DSTD),)
PLUGD  = usr/lib/$(shell uname -i)-linux-gnu/alsa-lib
else
PLUGD  = usr/lib/$(DSTD)/alsa-lib
endif


include ../lib/makedef.mk

CC        = gcc
CFLAGS    = -O2 -Wall -fdata-sections -ffunction-sections
LDFLAGS   = -Wl,--exclude-libs,ALL,-gc-sections

PLIBS     = -lconvert -lfft -lriffio -lmathex -lcmdline -lm -lasound
ALIBS     = -lmathex -lcmdline -lthr -lasound -lm -lpthread


all: mk_dir mk_lib $(PLUG) $(ARATE) post_make

debug: CFLAGS += -DDEBUG
debug: all

dump: CFLAGS += -DDUMP
dump: all


mk_dir:
	mkdir -p $(PLUGD)
	mkdir -p $(BIND)
	mkdir -p $(OBJD)

post_make:
	chmod 644 $(PLUGD)/$(PLUG).so
	cp $(MSRCD)/$(MIXERC) $(BIND)


mk_lib:
	( cd ../lib; $(MAKE) -f $(MAKEF) )

$(PLUG): $(PFILES)
	$(CC) -s -o $(PLUGD)/$(PLUG).so -shared $(LDFLAGS) $(addprefix $(OBJD)/, $(PFILES)) -L$(LIBD) $(PLIBS)

$(ARATE): $(AFILES)
	$(CC) -s -o $(BIND)/$(ARATE) $(LDFLAGS) $(addprefix $(OBJD)/, $(AFILES)) -L$(LIBD) $(ALIBS)

$(PFILES):
	$(CC) $(CFLAGS) $(INC) -fPIC -c $(PSRCD)/$*.c -o $(OBJD)/$@

$(AFILES):
	$(CC) $(CFLAGS) $(INC) -c $(ASRCD)/$*.c -o $(OBJD)/$@


purge: clean
	( cd ../lib; $(MAKE) -f $(MAKEF) purge )
	rm -rf usr
	rm -rf bin


clean:
	rm -rf $(OBJD)

	rm -rf $(DEBD)/tmp
	rm -rf $(DEBD)/*-utils
	rm -f $(DEBD)/*substvars
	rm -f $(DEBD)/*.log
	rm -f $(DEBD)/files
	rm -f build-stamp


install:
	cp $(ETCD)/$(CONF) /$(ETCD)
	cp $(PLUGD)/$(PLUG).so /$(PLUGD)
	if test -d /$(COMPTD); then ln -s /$(PLUGD)/$(PLUG).so /$(COMPTD); fi

	cp $(BIND)/$(ARATE) /$(BIND)
	cp $(BIND)/$(MIXERC) /$(BIND)


uninstall:
	rm -f /$(ETCD)/$(CONF)
	rm -f /$(PLUGD)/$(PLUG).so
	rm -f /$(COMPTD)/$(PLUG).so
	if test -d /$(COMPTD); then rmdir --ignore-fail-on-non-empty /$(COMPTD); fi

	rm -f /$(BIND)/$(ARATE)
	rm -f /$(BIND)/$(MIXERC)
