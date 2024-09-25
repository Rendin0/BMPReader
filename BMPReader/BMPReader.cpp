#include "BMPReader.h"

#include <iostream>

BMPReader::BMPReader()
{
	// Если сумма RGB равна этому числу, то пиксель должен быть белым, если нулю - то чёрным.
	// Если попытаться обратиться к значению по другому ключу, то выведется char(0)
	colors.emplace(0xFF + 0xFF + 0xFF, '#');
	colors.emplace(0, '.');
}

BMPReader::~BMPReader()
{
	closeBMP();
}

void BMPReader::openBMP(const std::string& path)
{
	bmpFile.open(path);

	if (!bmpFile.is_open())
	{
		std::cout << "\nCan't open the file\n";
		return;
	}

	// Запись данных в структуры. std::ifstream::read принимает первым параметром только char*, так что передаём в параметрах 
	// структуры в представлении char* 
	bmpFile.read(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
	bmpFile.read(reinterpret_cast<char*>(&infoHeader), sizeof(infoHeader));

	if (fileHeader.bfType != 0x4D42)
	{
		std::cout << "\nNot BMP format\n";
		closeBMP();
		return;
	}

	if (infoHeader.biBitCount != 24 && infoHeader.biBitCount != 32)
	{
		std::cout << "\nThis program can only read 24 and 32 bits bmp files (this file is " << infoHeader.biBitCount << ")\n";
		closeBMP();
		return;
	}
}

void BMPReader::displayBMP()
{
	if (!bmpFile.is_open())
	{
		std::cout << "\nFile is not open\n";
		return;
	}

	std::cout << std::endl;

	// Преобразуем размер окна консоли под размер картинки. Это в первую очередь нужно, чтобы избежать перенсов строк, т.к. они очень долго выполняются
	changeConsoleSize();

	if (infoHeader.biBitCount == 24)
		read24Bit();
	else
		read32Bit();
}

void BMPReader::closeBMP()
{
	if (bmpFile.is_open())
		bmpFile.close();

	fileHeader = BITMAPFILEHEADER{};
	infoHeader = BITMAPINFOHEADER{};
}

void BMPReader::read24Bit()
{
	RGBTRIPLE pixel{};


	// В формате bmp пиксели идут начиная с левого нижнего, так что приходится читать в таком порядке
	bmpFile.seekg(0-(sizeof(pixel) * infoHeader.biWidth), std::ios::end);

	for (int line = 0; line < infoHeader.biHeight; line++)
	{
		for (int column = 0; column != infoHeader.biWidth; column++)
		{
			bmpFile.read(reinterpret_cast<char*>(&pixel), sizeof(pixel));
			std::cout << colors[pixel.rgbtBlue + pixel.rgbtGreen + pixel.rgbtRed];
		}
		bmpFile.seekg(0-(sizeof(pixel) * infoHeader.biWidth) * 2, std::ios::cur);
	}
}

void BMPReader::read32Bit()
{
	RGBQUAD pixel{};
	bmpFile.seekg(0-(sizeof(pixel) * infoHeader.biWidth), std::ios::end);

	for (int line = 0; line < infoHeader.biHeight; line++)
	{
		for (int column = 0; column != infoHeader.biWidth; column++)
		{
			bmpFile.read(reinterpret_cast<char*>(&pixel), sizeof(pixel));
			std::cout << colors[pixel.rgbBlue + pixel.rgbGreen + pixel.rgbRed];
		}
		bmpFile.seekg(0-(sizeof(pixel) * infoHeader.biWidth) * 2, std::ios::cur);
	}
}

void BMPReader::changeConsoleSize()
{
	HWND console = GetConsoleWindow();
	RECT rect;
	CONSOLE_FONT_INFO fontInfo;
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);

	GetCurrentConsoleFont(handle, FALSE, &fontInfo);
	GetWindowRect(console, &rect);

	int consoleWidth = (infoHeader.biWidth + 5) * fontInfo.dwFontSize.X;
	int consoleHeight = (infoHeader.biHeight + 3) * fontInfo.dwFontSize.Y;

	MoveWindow(console, rect.left, rect.top, consoleWidth, consoleHeight, TRUE);
}
