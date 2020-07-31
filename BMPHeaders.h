#pragma once

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