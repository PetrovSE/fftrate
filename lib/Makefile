PROJS  = cmdline convert fft mathex net profiler riffio thr wavedev videodev
MODULE = _build
FILES  = _build_main.o
LIBS   = -lm -lasound -lpthread

PROJD  = .
CORED  = $(PROJD)/cl
SRCD   = ../src

include makedef.mk

TARGET = $(shell uname -i)
TGTD   = $(BINLIBD)-$(TARGET)


all: mk_dir
	$(call mk_library)
	$(call mk_lnk)

full: m32 m64

m32: mk_dir
	$(call mk_library, i386)
	$(call mk_lnk)

m64: mk_dir
	$(call mk_library, x86_64)
	$(call mk_lnk)


mk_dir:
	mkdir -p $(BIND)
	mkdir -p $(CORED)/$(OBJD)

mk_test: $(MODULE)


define mk_projects
	for proj in $(PROJS);					\
	do							\
		( cd $(SRCD)/$$proj; $(MAKE) $1 TARGET="$2" );	\
	done
endef

define mk_library
	$(call mk_projects, lib, $1)
	$(call mk_projects, solib, $1)
endef

define mk_lnk
	if [ -d $(TGTD) ]; then rm -rf $(BINLIBD); ln -s $(TGTD) $(BINLIBD); fi
	if [ -d $(BINLIBD) ] && [ ! -f $(BIND)/$(MODULE) ]; then ( $(MAKE) mk_test ); fi
endef


$(MODULE): $(FILES)
	$(CC) -o $(BIND)/$(MODULE) $(LDFLAGS) $(addprefix $(CORED)/$(OBJD)/, $(FILES)) $(BIND)/linux/*.a $(LIBS)

$(FILES):
	$(CC) $(CFLAGS) -c $(addprefix $(PROJD)/, $(basename $@).c) -o $(CORED)/$(OBJD)/$@


clean:
	for proj in $(PROJS);				\
	do						\
	    ( cd $(SRCD)/$$proj; $(MAKE) clean );	\
	done

	rm -rf $(CORED)/$(OBJD)
	rm -rf $(CORED)/cl
	rm -rf $(CORED)/Release*
	rm -rf $(CORED)/Debug*
	rm -rf $(CORED)/.build-*
	rm -rf $(CORED)/.clang

	rm -f $(CORED)/Makefile
	rm -f $(CORED)/*.mk
	rm -f $(CORED)/*.txt
	rm -f $(CORED)/*.session
	rm -f $(CORED)/*.tags


purge: clean
	rm -rf $(BIND)
