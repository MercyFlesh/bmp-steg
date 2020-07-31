#include "BMPSteg.h"
#include <string>

using namespace std;

int main(int argc, char* argv[])
{
	string path;
	string hidden_text;
	
	cout << "\x1b[32m[+]\x1b[0m Enter file path > ";
	cin >> path;

	cout << "\x1b[32m[+]\x1b[0m Enter hidden text > ";
	cin.ignore();
	getline(cin, hidden_text);
	
	try
	{
		BMPSteg bmp_file(path);
		bmp_file.read();
		bmp_file.hide(hidden_text);
		
		cout << "\x1b[32m[+]\x1b[0m Your text hidden to " << path << endl;
	}
	catch (const exception& ex)
	{
		cout << "\x1b[31m[-]\x1b[0m " << ex.what() << endl;
	}
	
	return 0;
}