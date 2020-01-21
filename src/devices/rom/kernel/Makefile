all: E64-II_kernel.bin bin2hex_64k kernel.cpp

bin2hex_64k: bin2hex_64k.c
	gcc -o bin2hex_64k bin2hex_64k.c

E64-II_kernel.bin: E64-II_kernel.asm E64-II_kernel_definitions.asm E64-II_kernel_tables.asm
	vasmm68k_mot -Fbin -m68000 -L E64-II_kernel.lst -o E64-II_kernel.bin E64-II_kernel.asm

kernel.cpp: E64-II_kernel.bin
	./bin2hex_64k > kernel.cpp

clean:
	rm *.lst
	rm *.bin
	rm kernel.cpp
	rm bin2hex_64k