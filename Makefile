

all: nano test

nano:
	make -f Makefile_nano

test:
	make -f Makefile_ut

clean:
	make -f Makefile_nano clean
	make -f Makefile_ut clean
