all:
	clang -Wall -O2 -lJudy as_build.c -o as_build
	clang -Wall -O2 as_show.c -o as_show `pkg-config --libs --cflags gtk+-3.0`
debug:
	clang -Wall -g -lJudy as_build.c -o as_build
	clang -Wall -g as_show.c -o as_show `pkg-config --libs --cflags gtk+-3.0`
build:
	./as_build ../../3-gram/ ../../data/temp/ ../mnt/key.bin ../mnt/value.bin
show:
	./as_show ../mnt/key.bin ../mnt/value.bin
clean:
	rm as_build as_show
