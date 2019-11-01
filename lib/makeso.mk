
all: mk_dir $(MODULE)

mk_dir:
	mkdir -p $(OBJD)
	mkdir -p $(BIND)


$(MODULE): $(FILES)
	$(CC) -s -o $(BIND)/lib$(MODULE).so -shared  $(addprefix $(OBJD)/, $(FILES)) -L $(BIND) $(LIBS)
	chmod -x $(BIND)/lib$(MODULE).so

$(FILES):
	$(CC) $(CFLAGS) -I $(INCD) -c $*.c -o $(OBJD)/$@


clean:
	rm -rf $(OBJD)
