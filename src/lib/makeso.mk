
all: mk_dir $(MODULE)

mk_dir:
	mkdir -p $(OBJD)
	mkdir -p $(BIND)


$(MODULE): $(FILES)
	$(CC) -s -o $(BIND)/lib$(MODULE).so -shared  $(addprefix $(OBJD)/, $(FILES)) -L $(LIBD) $(LIBS)
	chmod -x $(BIND)/lib$(MODULE).so

$(FILES):
	$(CMP) $(CFLAGS) -I $(INCD) -c $*.$(EXT) -o $(OBJD)/$@


clean:
	rm -rf $(OBJD)
