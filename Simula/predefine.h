#include "linalg.h"
#include "ap.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <ctime>
#include <cstdlib>
#include <itpp/itcomm.h>
#include <bitset>
#define L 32
#define S 256
#define BS 1024
using namespace alglib;
using namespace std;
using namespace itpp;
class epsilon1
{
public:
	float* codebook[S];
	void init(char* file_name);
	float distance(float* u, float *v);
	int nearest_index(float* u);
	void minus(float* u, float *v, float *ret);
};
class G
{
public:
	void matrix_multiplication(real_2d_array& a, real_2d_array& b, real_2d_array& r, ae_int_t m, ae_int_t n, ae_int_t k);
	bool eigen_values_and_vector(real_2d_array& a, ae_int_t n, ae_int_t s, real_1d_array& rw, real_1d_array& lw, 
								real_2d_array& vr, real_2d_array& vl);
};

class epsilon2_sk
{
public:
	void char_array_2_bvec(char* c, int n, bvec& bv);
	void encode(char* s1, vec& trans_symbols);
};

class sigma2
{
public:
	vec receive_symbols;
	Convolutional_Code conv_code;
	char ret[128];
	bvec decoded_bits;
	sigma2(vec rs);
	void decode();
	char bvec2char(bvec &bv, int pos);
	void write_file(bvec &bv, FILE* f, int t);
	unsigned char* seek_code_book();
};
void init(char* fn, std::ifstream& inf);
void read_a_vector(float* f, std::ifstream& inf);
int i_calculate(float* f, epsilon1& esp);
void e_calculate(float* f, int codeword_id, float* e, epsilon1& esp);
void append(float* from, float *to, int pos, int length);
void alpha(float *des, float *src, int* a, int n);
void display_vector(float* v, FILE* f);
void decode(vec& trans_symbols, epsilon1& esp);
void encode(epsilon1& esp, std::ifstream& inf, vec &v_ret, unsigned char* I, int* alpha_parameter);
