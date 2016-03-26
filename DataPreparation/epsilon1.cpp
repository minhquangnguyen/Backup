#include "predefine.h"
using namespace std;

float* codebook[S];

void epsilon1::init(char* file_name)
{
	for(int i = 0; i < S; i++)
		codebook[i] = new float[L];
	
	std::ifstream inf;
	inf.open(file_name, std::ios::in | std::ios::binary);
	int i = 0, j = 0;
	do
	{
		inf.read((char*)&codebook[i][j++], sizeof(float));
		if(j == L)
		{
			i++;
			j = 0;
		}
		if(inf.eof())
			break;
	}
	while(!inf.eof());
	cout << endl << i << endl;
	inf.close();
	return;
}

float epsilon1::distance(float* u, float *v)
{
	double sum = 0;
	for(int i = 0; i < L; i++)
	{
		sum += (u[i] - v[i]) * (u[i] - v[i]);
	}
	return sum;
}

int epsilon1::nearest_index(float* u)
{
	int min_index = 0;
	float min_value = 0, t;
	for(int i = 0; i < L; i++)
	{
		t = distance(u, codebook[i]);
		if(t < min_value)
		{
			min_index = i;
			min_value = t;
		}
	}
	return min_index;
}

void minus(float* u, float *v, float *ret)
{
	for(int i = 0; i < L; i++)
	{
		ret[i] += (u[i] - v[i]);
	}
	return;
}

int main(int argc, char* argv[])
{
	init(argv[1]);
	
	return 0;
}

