#include "predefine.h"
std::ifstream inf;
epsilon1 esp;
unsigned char I[128];

int alpha_parameter[] = {0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30};

// first arg is codebook, second is data to send.
int main(int argc, char* argv[])
{
	init(argv[2], inf);
	esp.init(argv[1]);
	vec v;

	encode(esp, inf, v, I, alpha_parameter);
	decode(v, esp);
	/*FILE* f;
	f = fopen("original.txt", "w");
	int t_id;
	float x[L], e[L], z[16];
	for(int i = 0; i < 128; i++)
	{
		read_a_vector(x, inf);
		//display_vector(x, f);
		t_id = esp.nearest_index(x);
		display_vector(esp.codebook[t_id], f);
		I[i] = t_id;
		//cout << t_id << " - ";
		e_calculate(x, t_id, e, esp);
		//display_vector(e, f);
		alpha(z, e, alpha_parameter, 16);
		append(z, E, i * 16, 16);
	}
	fclose(f);


	
	
	epsilon2_sk esk;
	vec trans_symbols;

	esk.encode((char *)I, trans_symbols);
	
	for(int i = 0; i < 2048; i++)
	{
		if(trans_symbols(i) > 1)
			trans_symbols(i) = trans_symbols(i) + 0.5;
		else
			trans_symbols(i) = trans_symbols(i) + 0.5;
		//trans_symbols(i) = trans_symbols(i) + E[i];
	}

	//cout << trans_symbols << endl;
	
	sigma2 s2(trans_symbols);
	s2.decode();

	//cout << "decoded_bits: " << s2.decoded_bits << endl;

	unsigned char* c = s2.seek_code_book();

	int t;
	FILE* f1 = fopen("destination.txt", "w");
	for(int i = 0; i < 128; i++)
	{
		t = (int)c[i];
		display_vector(esp.codebook[t], f1);
	}
	fclose(f1);
	inf.close();*/
	
	return 0;
}

/*




void display_codebook()
{
	for(int i = 0; i < 256; i++)
	{
		for(int j = 0; j < 32; j++)
			cout << esp.codebook[i][j] << " ";
		cout << endl;
	}
}
*/

