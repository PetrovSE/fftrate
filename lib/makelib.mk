
all: mk_dir $(MODULE)

mk_dir:
	mkdir -p $(OBJD)
	mkdir -p ../$(BIND)


$(MODULE): $(FILES)
	$(AR) ../$(BIND)/lib$(MODULE).a $(addprefix $(OBJD)/, $(FILES))

$(FILES):
	$(CC) $(CFLAGS) -c $*.c -o $(OBJD)/$@


clean:
	rm -rf $(OBJD)
