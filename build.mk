# Some make rules to make output pretty....

# default ARFLAGS also has 'v', but we don't want it to be verbose.
ARFLAGS= -r


CFLAGS+= -MT $@ -MMD -MP -MF $(BUILD_DIR)$*.d

$(BUILD_DIR)%.o : %.c
	@echo "COMPILE: $<"
	@$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)%.o : %.S
	@echo "ASSEMBLE: $<"
	@$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)%: $(BUILD_DIR)%.o
	@echo "   LINK: $@"
	@$(LINK.o) $^ -Xlinker -Map=$(BUILD_DIR)$*.map -o $@
	@$(SIZE) $@

$(BUILD_DIR)%.hex: $(BUILD_DIR)%
	@echo "OBJCOPY: $@"
	@$(OBJCOPY) -O ihex -R .eeprom -R .fuse -R .lock -R .signature $< $@

$(BUILD_DIR)%.eep: $(BUILD_DIR)%
	@echo "OBJCOPY: $@"
	@$(OBJCOPY) -O ihex -j .eeprom --set-section-flags=.eeprom="alloc,load" --change-section-lma .eeprom=0 --no-change-warnings $< $@

$(BUILD_DIR)(%): $(BUILD_DIR)%
	@echo "ARCHIVE: $< in $@"
	@$(AR) $(ARFLAGS) $@ $<

-include $(BUILD_DIR)*.d
