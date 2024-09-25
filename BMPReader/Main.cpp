#include "BMPReader.h"

int main(int argc, char** argv)
{
	if (argc == 1)
		return 0;

	BMPReader reader{};
	reader.openBMP(argv[1]);
	reader.displayBMP();
	reader.closeBMP();

}