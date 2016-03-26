#include <iostream>
#include <random>
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

	std::default_random_engine generator;
	std::normal_distribution<double> distribution(0, 2.0);

	int p[10] = {};
	float f;
	// Tao ra 100000 vector

	for (int i = 0; i < 3200000; ++i) 
	{
		double number = distribution(generator);
		f = number;
		//cout << f << " ";
		out.write( reinterpret_cast<const char*>( &f ), sizeof( float ));
		if ((number>=-5.0)&&(number<5.0)) 
			++p[int(number + 5)];
	}

	out.close();

	std::cout << "normal_distribution (5.0,2.0):" << std::endl;

	for (int i=0; i<10; ++i) 
	{
		std::cout << i << "-" << (i+1) << ": ";
		std::cout << std::string(p[i]*0.001,'*') << std::endl;
	}

	return 0;
}

// training: ./lbg -n 31 -e 256 -t 100000 -i 100 -m 10 -S 0 < train.dat > cbfile
