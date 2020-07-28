#include <iostream>
#include <fstream>
#include <vector>
#include <bitset>

using namespace std;


struct BITMAPFILEHEADER
{
	short Type;			// 0x4d42
	int file_Size;		// размер файла
	int Reserved;	    // 0
	int OffsetData;     // смещение до поля данных, обычно 54 = 14 + biSize
};


struct BITMAPINFOHEADER
{
	int Header_Size;		// размер струкуры в байтах 40(BITMAPINFOHEADER) или 108(BITMAPV4HEADER или 124(BITMAPV5HEADER)
	int Width;				// ширина картинки
	int Height;				// высота картинки
	short Planes;			// всегда должно быть 1
	short BitCount;			// 1 | 4 | 8 | 16 | 24 | 32
	int Compression;		// BI_RGB | BI_RLE8 | BI_RLE4 | BI_BITFIELDS | BI_JPEG | BI_PNG
	int ImageSize;			// Количество байт в поле данных 
	int XPelsPerMeter;		// горизонтальное разрешение, точек на дюйм
	int YPelsPerMeter;		// вертикальное разрешение, точек на дюйм
	int UsedColors;			// Количество используемых цветов
	int ImportantColors;    // Количество существенных цветов. Можно считать, просто 0
};


struct BMPColorHeader
{
	int	RedMask;			// Bit mask for the red channel
	int GreenMask;			// Bit mask for the green channel
	int BlueMask;			// Bit mask for the blue channel
	int AlphaMask;			// Bit mask for the alpha channel
	int ColorSpace_type;	// Default "sRGB" (0x73524742)
	int Unused[16];			// Unused data for sRGB color space
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
	BMPColorHeader color_header;
	vector<uint8_t> data;
	const string path;
};


void BMP::read()
{
	ifstream bmp;
	bmp.exceptions(ifstream::badbit | ifstream::failbit);

	try
	{
		bmp.open(path, ios::binary);

		bmp.read(reinterpret_cast<char*>(&file_header), sizeof(file_header));
		if (file_header.Type != 19778)
		{
			throw runtime_error("Error! Invalid file format. Use only bmp");
		}

		bmp.read(reinterpret_cast<char*>(&info_header), sizeof(info_header));

		{
			bmp.seekg(0, ios::end);
			int filesize = bmp.tellg();
			bmp.seekg(file_header.OffsetData, ios::beg);

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


int main()
{
	BMP bmp_file("new.bmp");
	return 0;
}