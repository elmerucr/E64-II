all: kernel.bin bin2hex_64k kernel.cpp

bin2hex_64k: bin2hex_64k.c
	gcc -o bin2hex_64k bin2hex_64k.c

kernel.bin: kernel.s kernel_definitions.s kernel_tables.s
	vasmm68k_mot -align -Fbin -m68000 -L kernel.lst -o kernel.bin kernel.s

kernel.cpp: kernel.bin
	./bin2hex_64k > kernel.cpp

clean:
	rm *.lst
	rm *.bin
	rm kernel.cpp
	rm bin2hex_64k