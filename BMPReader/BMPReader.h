#pragma once
#include <string>
#include <fstream>
#include <map>
#include <Windows.h>

class BMPReader
{
public:
	BMPReader();
	~BMPReader();

	void openBMP(const std::string& path);
	void displayBMP();
	void closeBMP();


private:
	void read24Bit();
	void read32Bit();
	void changeConsoleSize();

	std::map<long, char> colors{};

	std::ifstream bmpFile{};

	BITMAPFILEHEADER fileHeader{};
	BITMAPINFOHEADER infoHeader{};
};

