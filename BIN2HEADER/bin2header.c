#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
	if (argc < 4) {
		fprintf(stderr, "Usage: %s <input_file> <output_file> <array_name>\n", argv[0]);
		return 1;
	}

	char *input_path = argv[1];
	char *output_path = argv[2];
	char *array_name = argv[3];

	FILE *fin = fopen(input_path, "rb");
	if (!fin) {
		perror("Failed to open input file");
		return 1;
	}

	FILE *fout = fopen(output_path, "w");
	if (!fout) {
		perror("Failed to open output file");
		fclose(fin);
		return 1;
	}

	// ヘッダーの書き出し
	fprintf(fout, "/* Generated from: %s */\n", input_path);
	fprintf(fout, "unsigned char %s[] = {\n	", array_name);

	int ch;
	unsigned long count = 0;
	while ((ch = fgetc(fin)) != EOF) {
		fprintf(fout, "0x%02X, ", (unsigned char)ch);
		count++;

		// 8個ごとに改行して見やすくする
		if (count % 8 == 0) {
			fprintf(fout, "\n	");
		}
	}

	fprintf(fout, "\n};\n");
	fprintf(fout, "unsigned int %s_len = %lu;\n", array_name, count);

	fclose(fin);
	fclose(fout);

	printf("Success: Generated %s (%lu bytes)\n", output_path, count);
	return 0;
}