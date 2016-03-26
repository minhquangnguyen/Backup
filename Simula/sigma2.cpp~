#include "predefine.h"
sigma2::sigma2(vec rs)
{
	receive_symbols = rs;
	ivec generators;
	int constraint_length;	
	generators.set_size(2, false);
	generators(0) = 0133;
	generators(1) = 0145;
	constraint_length = 7;
	conv_code.set_generator_polynomials(generators, constraint_length);
}

void sigma2::decode()
{
	decoded_bits = conv_code.decode(receive_symbols);
	return;
}

char sigma2::bvec2char(bvec &bv, int pos)
{
	char ret = 0;
	int a[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
	if(pos > bv.length() - 8)
	{
		cout << "Can sua loi nay";
		return 0;
	}
	for(int i = pos; i < pos + 8; i++)
	{
		if(bv[i] == 1)
			ret = ret | a[7 - i + pos];
	}
	return ret;
}

unsigned char* sigma2::seek_code_book()
{
	unsigned char* c = new unsigned char[128];
	for(int j = 0; j < 128; j++)
	{
		c[j] = (unsigned char)bvec2char(decoded_bits, j * 8);
	}
	return c;
}

void sigma2::write_file(bvec &bv, FILE* f, int t)
{
	char b;
	for(int j = 0; j < t/8; j++)
	{
		b = bvec2char(bv, j * 8);
		fwrite(&b, 1, 1, f);
	}
	return;
}
