// elmerucr - 2020-01-03
// convert a binary 64k rom file to ascii hex format, 16 bytes a row

#include <stdint.h>
#include <stdio.h>

int main() {
	uint8_t romdata[2 * 65536];
	FILE *f;
	f = fopen("kernel.bin", "r");
	fread(romdata, 2 * 65536, 1, f);
	fclose(f);
	printf("// 64k E64-II kernel elmerucr (c)2020\n");
	printf("//\n\n");
	printf("#include <cstdint>\n\n");
	printf("uint8_t kernel[65536] =\n{");

	for(int i = 65536; i<(2 * 65535); i++) {
		if(i%16 == 0) printf("\n\t");
		printf("0x%02x,", romdata[i]);
	}
	printf("0x%02x", romdata[(2*65536)-1]);
	printf("\n};\n");
	return 0;
}
