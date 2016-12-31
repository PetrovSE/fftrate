CFLAGS = $(COPT)

all: mk_dir $(MODULE)

mk_dir:
	mkdir -p $(OBJD)
	mkdir -p ../$(BIND)

$(MODULE): $(FILES)
	$(CMP) $(LDFLAGS) $(OBJD)/* "-L../$(BINLIBD)" $(LIBS) -o ../$(BIND)/$(MODULE)

$(FILES):
	$(CMP) $(CFLAGS) -c $*.$(EXT) -o $(OBJD)/$(notdir $@)

clean:
	rm -rf $(OBJD)
	rm -rf Release*
	rm -rf Debug*

	rm -f $(MODULE).mk
	rm -f $(MODULE).txt
