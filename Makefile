

all: nano tiny test

nano:
	make -f Makefile_nano

tiny:
	make -f Makefile_tiny

test:
	make -f Makefile_ut

clean:
	make -f Makefile_nano clean
	make -f Makefile_tiny clean
	make -f Makefile_ut clean
