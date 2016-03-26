#include <iostream>
#include <fstream>
#include <stdio.h>
#include <ctime>
#include <cstdlib>

using namespace std;

int main()
{
	std::ofstream out;
	out.open("train.dat", std::ios::out | std::ios::binary);
	srand (static_cast <unsigned> (time(0)));
	float f = 0.5, LO = 0, HI = 10;
	for(int i = 0; i < 320000; i++)
	{
		float f = LO + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(HI-LO)));
		out.write( reinterpret_cast<const char*>( &f ), sizeof( float ));
		//cout << f << endl;
	}	

	out.close();
	return 0;
}
/*


	float f[] = {100, 0.1, 101, 0.2, 102.3, 0.12, 103.4, 0.25, 0.09, 99.1, 0.12, 98.3, 0.17, 97.5, 0.1, 99.5};
	for(int i = 0; i < 16; i++)
		out.write( reinterpret_cast<const char*>( &f[i]), sizeof( float ));

	./lbg -n 31 -e 256 -t  -i 100 -m 10 -S 0 < bin.dat > cbfile
*/
