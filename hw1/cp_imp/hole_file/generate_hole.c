#include <stdio.h>
#include <unistd.h>
#include <sys/random.h>
#define BUFFER_SIZE 4096
#define NUM_HOLES 3
#define GAP 1024 // bytes

int main() {
	FILE* out_fp = fopen("file_with_holes", "wb");
	char buffer[BUFFER_SIZE];
	for (int i = 0; i < BUFFER_SIZE; ++i) buffer[i] = '#';

	for (int i = 0; i < NUM_HOLES; ++i)
	{
		//getrandom(buffer, sizeof(buffer), 0);
		fwrite(buffer, sizeof(char), BUFFER_SIZE, out_fp);
		fseek(out_fp, GAP, SEEK_CUR);
	}
	//getrandom(buffer, sizeof(buffer), 0);
	fwrite(buffer, sizeof(char), BUFFER_SIZE, out_fp);
	fclose(out_fp);
	return 0;
}
