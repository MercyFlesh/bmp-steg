#include <iostream>
#include <fstream>
#include <vector>
#include <bitset>
#include <string>
#include <cstdint>

using namespace std;

#define FILE_HEADER_SIZE 14
#define INFO_HEADER_SIZE 40


struct BITMAPFILEHEADER
{
	short Type;		
	int file_Size;  
	int Reserved;   
	int OffsetData; 
};


struct BITMAPINFOHEADER
{
	int Header_Size;
	int Width;                      
	int Height;                     
	short Planes;                   
	short BitCount;                 
	int Compression;                
	int ImageSize;                  
	int XPelsPerMeter;              
	int YPelsPerMeter;              
	int UsedColors;                 
	int ImportantColor;             
};


class BMP
{
public:
	BMP(const string& path) 
		: path(path){}

	void read();
	
	void write(const int& width, const int& height);

private:
	BITMAPFILEHEADER file_header;
	BITMAPINFOHEADER info_header;
	vector<uint8_t> data;
	const string path;

	int read_inversed_data(const unsigned char* arr, const int& amount) const;
	void filling_headers(const unsigned char* headers_data);
	void valid_bmp(ifstream& bmp) const;
};


int BMP::read_inversed_data(const unsigned char arr[], const int& amount) const
{
	int result = 0;
	for (size_t i = 0; i < amount; i++)
	{
		result += (arr[i] << (i * 8));
	}

	return result;
}


void BMP::valid_bmp(ifstream& bmp) const
{
	bmp.seekg(0, SEEK_END);
	int filesize = bmp.tellg();
	bmp.seekg(file_header.OffsetData);


	if (file_header.Type != 19778)
	{
		throw runtime_error("Invalid file format. Use only bmp");
	}

	if (file_header.file_Size != filesize || file_header.Reserved != 0 || info_header.Planes != 1 ||
		(info_header.Header_Size != 40 && info_header.Header_Size != 108 && info_header.Header_Size != 124) ||
		file_header.OffsetData != 14 + info_header.Header_Size || info_header.Width < 1 || info_header.Height < 1)
	{
		throw runtime_error("Incorrect bmp file");
	}

	if ((info_header.BitCount != 24) || info_header.Compression != 0)
	{
		throw runtime_error("Unsupported bmp type");
	}
}


void BMP::filling_headers(const unsigned char headers_data[])
{
	file_header.Type = read_inversed_data(headers_data, 2);
	file_header.file_Size = read_inversed_data(headers_data + 2, 4);
	file_header.Reserved = read_inversed_data(headers_data + 6, 4);
	file_header.OffsetData = read_inversed_data(headers_data + 10, 4);

	info_header.Header_Size = read_inversed_data(headers_data + 14, 4);
	info_header.Width = read_inversed_data(headers_data + 18, 4);
	info_header.Height = read_inversed_data(headers_data + 22, 4);
	info_header.Planes = read_inversed_data(headers_data + 26, 2);
	info_header.BitCount = read_inversed_data(headers_data + 28, 2);
	info_header.Compression = read_inversed_data(headers_data + 30, 4);
	info_header.ImageSize = read_inversed_data(headers_data + 34, 4);
	info_header.XPelsPerMeter = read_inversed_data(headers_data + 38, 4);
	info_header.YPelsPerMeter = read_inversed_data(headers_data + 42, 4);
	info_header.UsedColors = read_inversed_data(headers_data + 46, 4);
	info_header.ImportantColor = read_inversed_data(headers_data + 50, 4);
}


void BMP::read()
{
	ifstream bmp;
	bmp.exceptions(ifstream::badbit | ifstream::failbit);

	unsigned char headers_data[FILE_HEADER_SIZE + INFO_HEADER_SIZE];

	try
	{
		bmp.open(path, ios::binary);
		bmp.read(reinterpret_cast<char*> (&headers_data), FILE_HEADER_SIZE);
		bmp.read(reinterpret_cast<char*> (&(*(headers_data + FILE_HEADER_SIZE))), INFO_HEADER_SIZE);
		
		filling_headers(headers_data);
		valid_bmp(bmp);

		data.resize(info_header.ImageSize);
		bmp.read(reinterpret_cast<char*> (data.data()), info_header.ImageSize);

		bmp.close();
	}
	catch (const ifstream::failure& ex)
	{
		bmp.close();
		throw ex;
	}
	catch (const exception& ex)
	{
		bmp.close();
		throw ex;
	}
}


int main(int argc, char* argv[])
{
	BMP bmp_file(argv[1]);
	try
	{
		bmp_file.read();
	}
	catch (const exception& ex)
	{
		cout << ex.what() << endl;
	}
	
	return 0;
}