MAKEFLAGS += --no-print-directory


all: nano tiny tiny2313 test

nano:
	@$(MAKE) -f Makefile_nano
	
tiny:
	@$(MAKE) -f Makefile_tiny

tiny2313:
	@$(MAKE) -f Makefile_tiny2313

test:
	@$(MAKE) -f Makefile_ut

clean:
	@$(MAKE) -f Makefile_nano clean
	@$(MAKE) -f Makefile_tiny clean
	@$(MAKE) -f Makefile_tiny2313 clean
	@$(MAKE) -f Makefile_ut clean
