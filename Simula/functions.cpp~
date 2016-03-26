#include "predefine.h"

void init(char* fn, std::ifstream& inf)
{
	inf.open(fn, std::ios::in | std::ios::binary);
}

void read_a_vector(float* f, std::ifstream& inf)
{
	for(int i = 0; i < L; i++)
	{
		inf.read((char*)&f[i], sizeof(float));
	}
	return;
}

int i_calculate(float* f, epsilon1& esp)
{
	return esp.nearest_index(f);
}

void e_calculate(float* f, int codeword_id, float* e, epsilon1& esp)
{
	for(int i = 0; i < L; i++)
	{
		e[i] = f[i] - esp.codebook[codeword_id][i];
	}
	return;
}

void append(float* from, float *to, int pos, int length)
{
	for(int i = 0; i < length; i++)
	{
		to[i + pos] = from[i];
	}
	return;
}

void alpha(float *des, float *src, int* a, int n)
{
	for(int i = 0; i < n; i++)
	{
		des[i] = src[a[i]];
	}
}

void display_vector(float* v, FILE* f)
{
	for(int i = 0; i < 32; i++)
	{
		//cout << v[i] << " ";
		fprintf(f,"%2.3lf\t", v[i]);
	}
	fprintf(f, "\n");
	return;
}

void encode(epsilon1& esp, std::ifstream& inf, vec &v_ret, unsigned char* I, int* alpha_parameter)
{
	float E[2048];
	FILE* f;
	f = fopen("original.txt", "w");
	int t_id;
	float x[L], e[L], z[16];
	for(int i = 0; i < 128; i++)
	{
		read_a_vector(x, inf);
		display_vector(x, f);
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


	
	esk.encode((char *)I, v_ret);
	for(int i = 0; i < 2048; i++)
	{
		v_ret(i) = v_ret(i) + E[i];
	}

	for(int i = 0; i < 2018; i++)
		cout << v_ret(i) << " ";
	cout << endl;
}

void decode(vec& trans_symbols, epsilon1& esp)
{
	epsilon2_sk esk;
	sigma2 s2(trans_symbols);
	
	s2.decode();
	unsigned char* J = s2.seek_code_book();

	vec temp_vec;
	esk.encode((char *)J, temp_vec);

	
	cout << "trans_symbols: " << trans_symbols << endl;
	vec e = trans_symbols - temp_vec;
	cout << "e: " << e  << endl;
	vec Z;
	int stemp = e.size();
	Z.set_size(stemp * 2);
	for(int i = 0; i < stemp; i++)
	{
		Z(2 * i) = e(i);
		Z(2 * i + 1) = 0;
		cout << Z(2 * i) << " " << Z(2 * i + 1) << " ";
	}
	cout << endl;

	FILE* f;
	f = fopen("Dest.txt", "w");
	for(int i = 0; i < 128; i++)
	{
		for(int j = 0; j < L; j++)
		{
			Z(i * L + j) = Z(i * L + j) + esp.codebook[J[i]][j];
			fprintf(f, "%2.3lf\t", Z(i * L + j));
		}
		fprintf(f, "\n");
	}
	fclose(f);
}
