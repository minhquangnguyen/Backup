#include <iostream>
#include <fstream>
#include <stdio.h>
using namespace std;
int main(int argc, char* argv[])
{
	std::ifstream inf;
	
	inf.open(argv[1], std::ios::in | std::ios::binary);
	float f = 0.5;
	int i = 0;
	std::cout.precision(4);
	do
	{
		
		inf.read((char*)&f, sizeof(float));
		printf( "%6.4f \n", f );
		i++;
		if(inf.eof())
			break;
	}
	while(!inf.eof());
	cout << endl << i << endl;
	
	inf.close();
	return 0;
}
