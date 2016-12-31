ifeq ($(TARGET),)
TARGETD = $(BINLIBD)
else
TARGETD = $(BINLIBD)-$(TARGET)
endif

ifeq ($(TARGET),i386)
CFLAGS  += -m32
LDFLAGS += -m32
endif

ifeq ($(TARGET),x86_64)
CFLAGS  += -m64
LDFLAGS += -m64
endif


all: lib
lib: mk_dir $(MODULE)

solib: MODULE := $(MODULE)_so
solib: CFLAGS += $(CSHARED)
solib: mk_dir $(MODULE)


mk_dir:
	mkdir -p $(OBJD)
	mkdir -p ../$(TARGETD)

$(MODULE): $(FILES)
	$(AR) ../$(TARGETD)/lib$(MODULE).a $(addprefix $(OBJD)/, $(FILES))

$(FILES):
	$(CC) $(CFLAGS) -c $*.c -o $(OBJD)/$@

clean:
	rm -rf $(OBJD)
	rm -rf Release*
	rm -rf Debug*
