#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRUE  1
#define FALSE 0

#define BAD_NUMBER_ARGS 1
#define FSEEK_ERROR 2
#define FREAD_ERROR 3
#define MALLOC_ERROR 4
#define FWRITE_ERROR 5

/**
 * Parses the command line.
 *
 * argc:      the number of items on the command line (and length of the
 *            argv array) including the executable
 * argv:      the array of arguments as strings (char* array)
 * grayscale: the integer value is set to TRUE if grayscale output indicated
 *            outherwise FALSE for threshold output
 *
 * returns the input file pointer (FILE*)
 **/
/*They gave us the below code until getIntensity function*/
FILE *parseCommandLine(int argc, char **argv, int *isGrayscale) {
	if (argc > 2) {
		printf("Usage: %s [-g]\n", argv[0]);
		exit(BAD_NUMBER_ARGS);
	}

	if (argc == 2 && strcmp(argv[1], "-g") == 0) {
		*isGrayscale = TRUE;
	}
	else {
		*isGrayscale = FALSE;
	}

	return stdin;
}

unsigned getFileSizeInBytes(FILE* stream) {
	unsigned fileSizeInBytes = 0;

	rewind(stream);
	if (fseek(stream, 0L, SEEK_END) != 0) {
		exit(FSEEK_ERROR);
	}
	fileSizeInBytes = ftell(stream);

	return fileSizeInBytes;
}

void getBmpFileAsBytes(unsigned char* ptr, unsigned fileSizeInBytes, FILE* stream) {
	rewind(stream);
	if (fread(ptr, fileSizeInBytes, 1, stream) != 1) {
#ifdef DEBUG
		printf("feof() = %x\n", feof(stream));
		printf("ferror() = %x\n", ferror(stream));
#endif
		exit(FREAD_ERROR);
	}
}


/*below is my code*/





unsigned char getAverageIntensity(unsigned char blue, unsigned char green, unsigned char red) {
	unsigned char intensity = (blue + green + red) / 3;
	return intensity;
}

void applyGrayscaleToPixel(unsigned char* pixel) {

	unsigned char blue = *(pixel);
	unsigned char green = *(pixel + 1);
	unsigned char red = *(pixel + 2);

	int Intensity = getAverageIntensity(blue, green, red);

	blue = Intensity;
	green = Intensity;
	red = Intensity;


}

void applyThresholdToPixel(unsigned char* pixel) {
	unsigned char blue = *(pixel);
	unsigned char green = *(pixel + 1);
	unsigned char red = *(pixel + 2);

	int Intensity = getAverageIntensity(blue, green, red);
	if (Intensity >= 128)
	{
		blue = 0xff;
		green = 0xff;
		red = 0xff;
	}
	else
	{
		blue = 0x00;

		green = 0x00;
		red = 0x00;
	}
}

void applyFilterToPixel(unsigned char* pixel, int isGrayscale) {
	if (isGrayscale) {
		applyGrayscaleToPixel(pixel);
	}
	else
	{
		applyThresholdToPixel(pixel);
	}

}

void applyFilterToRow(unsigned char* row, int width, int isGrayscale) {
	unsigned char* pixel = NULL;
	int pixlength = width * 3;
	for (int p = 0; p < pixlength; p++)
	{
		if (p == 0 || p % 3 == 0)
		{

			pixel = row + p;

			applyFilterToPixel(pixel, isGrayscale);

		}


	}
}

int padding(int width) {
	int a = 0;
	while (((width * 3) + a) % 4 != 0)
	{
		a++;
	}

	int newwidth = (width * 3) + a;
	return newwidth;

}

void applyFilterToPixelArray(unsigned char* pixelArray, int width, int height, int isGrayscale) {
	unsigned char* row = NULL;
	int pad = 0;
	int padd = FALSE;
	int parem = 0;

	if ((width * 3) % 4 != 0)
	{
		pad = padding(width);
		padd = TRUE;
	}

	for (int i = 0; i < height; i++)
	{
		if (padd) {
			parem = pad * i;
		}
		else
		{
			parem = width * i;
		}

		row = pixelArray + parem;
		applyFilterToRow(row, width, isGrayscale);
	}


}








/*They gave us the below code*/
void parseHeaderAndApplyFilter(unsigned char* bmpFileAsBytes, int isGrayscale) {
	int offsetFirstBytePixelArray = 0;
	int width = 0;
	int height = 0;
	unsigned char* pixelArray = NULL;
	offsetFirstBytePixelArray = *((int *)(bmpFileAsBytes + 10));
	width = *((int *)(bmpFileAsBytes + 18));
	height = *((int*)(bmpFileAsBytes + 22));
	pixelArray = bmpFileAsBytes + offsetFirstBytePixelArray;

#ifdef DEBUG
	printf("offsetFirstBytePixelArray = %u\n", offsetFirstBytePixelArray);
	printf("width = %u\n", width);
	printf("height = %u\n", height);
	printf("pixelArray = %p\n", pixelArray);
#endif

	applyFilterToPixelArray(pixelArray, width, height, isGrayscale);
}

int main(int argc, char **argv) {
	int grayscale = FALSE;
	unsigned fileSizeInBytes = 0;
	unsigned char* bmpFileAsBytes = NULL;
	FILE *stream = NULL;

	stream = parseCommandLine(argc, argv, &grayscale);
	fileSizeInBytes = getFileSizeInBytes(stream);

#ifdef DEBUG
	printf("fileSizeInBytes = %u\n", fileSizeInBytes);
#endif

	bmpFileAsBytes = (unsigned char *)malloc(fileSizeInBytes);
	if (bmpFileAsBytes == NULL) {
		exit(MALLOC_ERROR);
	}
	getBmpFileAsBytes(bmpFileAsBytes, fileSizeInBytes, stream);

	parseHeaderAndApplyFilter(bmpFileAsBytes, grayscale);

#ifndef DEBUG
	if (fwrite(bmpFileAsBytes, fileSizeInBytes, 1, stdout) != 1) {
		exit(FWRITE_ERROR);
	}
#endif

	free(bmpFileAsBytes);
	return 0;
}
