
all: mk_dir $(MODULE)

mk_dir:
	mkdir -p $(OBJD)
	mkdir -p ../$(BIND)


$(MODULE): $(FILES)
	$(CMP) $(LDFLAGS) $(OBJD)/* "-L../../lib/$(BIND)" $(LIBS) -o ../$(BIND)/$(MODULE)

$(FILES):
	$(CMP) $(CFLAGS) -c $*.$(EXT) -o $(OBJD)/$(notdir $@)


clean:
	rm -rf $(OBJD)