#include "predefine.h"
const double MIN_RAND = 2.0, MAX_RAND = 6.0;
const double range = MAX_RAND - MIN_RAND;

#define MS 4

void display_2d(real_2d_array &a, ae_int_t m, ae_int_t n)
{
	for(int i = 0; i < m; i++)
	{
		for(int j = 0; j < n; j++)
		{
			printf("%2.10f\t", a(i, j));
		}
		cout << endl;
	}
	return;
}

void display_1d(real_1d_array &a, ae_int_t n)
{
	for(int i = 0; i < n; i++)
		printf("%2.10f\t", a(i));
	cout << endl;
	return;
}

int main(int argc, char* argv[])
{
	G g;
	real_2d_array a, b, c;
	

	double random; 
	double d_array[MS];
	double _d_array[MS * MS];
	for(int i = 0; i < MS * MS; i++)
		_d_array[i] = 0.9;

	for(int i = 0; i < MS; i++)
		_d_array[i * MS + i] = 1.0;
	
	for(int i = 0; i < MS; i++)
		d_array[i] = 1; //range * ((((float) rand()) / (float) RAND_MAX)) + MIN_RAND;
	a.setlength(MS, 1);
	a.setcontent(MS, 1, d_array);

	for(int i = 0; i < MS; i++)
		d_array[i] = 3;//range * ((((float) rand()) / (float) RAND_MAX)) + MIN_RAND;
	b.setlength(1, MS);
	b.setcontent(1, MS, d_array);
	c.setlength(MS, MS);
	g.matrix_multiplication(a, b, c, MS, MS, 1);
	

	c.setcontent(MS, MS, _d_array);	
	display_2d(c, MS, MS);
	real_1d_array rw, iw;
	real_2d_array vr, vl;
	g.eigen_values_and_vector(c, MS, 3, rw, iw, vr, vl);
	cout << "real eigenvalue-----" << endl;
	display_1d(rw, MS);
	cout << "imagine eigenvalue-----" << endl;
	display_1d(iw, MS);
	cout << "eigenvector-----" << endl;
	display_2d(vr, MS, MS);
	cout << "-----" << endl;
	display_2d(vl, MS, MS);
	
	return 0;
}
