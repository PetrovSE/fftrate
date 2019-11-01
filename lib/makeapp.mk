
all: mk_dir $(MODULE)

mk_dir:
	mkdir -p $(OBJD)
	mkdir -p $(BIND)


$(MODULE): $(FILES)
	$(CMP) $(LDFLAGS) $(OBJD)/* -L $(BIND) $(LIBS) -o $(BIND)/$(MODULE)

$(FILES):
	$(CMP) $(CFLAGS) -I $(INCD) -c $*.$(EXT) -o $(OBJD)/$(notdir $@)


clean:
	rm -rf $(OBJD)

	rm -rf release
	rm -rf debug

	rm -f Makefile
	rm -f Makefile.*
