all: kernel.bin bin2hex_64k kernel.cpp

bin2hex_64k: bin2hex_64k.c
	gcc -o bin2hex_64k bin2hex_64k.c

kernel.bin: kernel.asm kernel_definitions.asm kernel_tables.asm
	vasmm68k_mot -Fbin -m68000 -L kernel.lst -o kernel.bin kernel.asm

kernel.cpp: kernel.bin
	./bin2hex_64k > kernel.cpp

clean:
	rm *.lst
	rm *.bin
	rm kernel.cpp
	rm bin2hex_64k