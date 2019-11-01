
all: mk_dir $(MODULE)

mk_dir:
	mkdir -p $(OBJD)
	mkdir -p $(LIBD)


$(MODULE): $(FILES)
	$(AR) rcus $(LIBD)/lib$(MODULE).a $(addprefix $(OBJD)/, $(FILES))

$(FILES):
	$(CC) $(CFLAGS) -I $(INCD) -c $*.c -o $(OBJD)/$@


clean:
	rm -rf $(OBJD)
