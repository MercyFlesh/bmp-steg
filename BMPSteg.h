#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include "BMPHeaders.h"

#define FILE_HEADER_SIZE 14
#define INFO_HEADER_SIZE 40

class BMPSteg
{
public:
	BMPSteg(const std::string& path)
		: path(path)
	{}

	void read();

	void hide(const std::string& written_data);

private:
	BITMAPFILEHEADER file_header;
	BITMAPINFOHEADER info_header;
	std::vector<uint8_t> data;
	const std::string path;

	int read_inversed_data(const unsigned char* arr, const int& amount) const;
	void filling_headers(const unsigned char* headers_data);
	void valid_bmp(std::ifstream& bmp) const;
};
