# Some make rules to make output pretty....

# default ARFLAGS also has 'v', but we don't want it to be verbose.
ARFLAGS= -r


%.o : %.c
	@echo "COMPILE: $<"
	@$(CC) $(CFLAGS) -c $< -o $@

%: %.o
	@echo "   LINK: $@"
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@

%.hex: %
	@echo "OBJCOPY: $@"
	@$(OBJCOPY) -O ihex -R .eeprom -R .fuse -R .lock -R .signature $< $@

(%): %
	@echo "ARCHIVE: $< in $@"
	@$(AR) $(ARFLAGS) $@ $<
