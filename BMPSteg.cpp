#include "BMPSteg.h"

using namespace std;


int BMPSteg::read_inversed_data(const unsigned char arr[], const int& amount) const
{
	int result = 0;
	for (size_t i = 0; i < amount; i++)
	{
		result += (arr[i] << (i * 8));
	}

	return result;
}


void BMPSteg::valid_bmp(ifstream& bmp) const
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


void BMPSteg::filling_headers(const unsigned char headers_data[])
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


void BMPSteg::read()
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


void BMPSteg::hide(const string& written_data)
{
	ofstream bmp;
	bmp.exceptions(ifstream::badbit | ifstream::failbit);

	try
	{
		int max_hiding_size = info_header.Width * info_header.Height * 3;
		if (sizeof(written_data) > max_hiding_size)
			throw runtime_error("very much size written text");

		vector<uint8_t>::iterator data_iter = data.begin();
		for (const char& symb : written_data)
		{
			*data_iter = symb;
			data_iter++;
		}

		bmp.open(path, ios::binary | ios::out | ios::in);
		bmp.seekp(file_header.OffsetData);
		copy(data.begin(), data.end(), ostreambuf_iterator<char>(bmp));
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
