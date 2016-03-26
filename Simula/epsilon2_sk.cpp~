#include "predefine.h"
void epsilon2_sk::char_array_2_bvec(char* c, int n, bvec& bv)
{
	
	int pos = 0;
	bvec a;
	for(int i = 0; i < n; i++)
	{
		a = itpp::dec2bin(8, (int) c[i]);
		bv.replace_mid(pos, a);
		pos += 8;
	}
	cout << endl;
	return;
}
void epsilon2_sk::encode(char* s1, vec& trans_symbols)
{
	int constraint_length, error = 0;
	double Ec, Eb, error_rate, _BER;
	
	ivec generators;
	vec EbN0dB, EbN0, N0, ber, rec_symbols;
	bvec coded_bits, decoded_bits;
	BPSK bpsk;
	BERC berc;
	AWGN_Channel channel;
	
	generators.set_size(2, false);
	generators(0) = 0133;
	generators(1) = 0145;
	constraint_length = 7;
	Convolutional_Code code;

	code.set_generator_polynomials(generators, constraint_length);
	
	Ec = 1.0;
	EbN0dB = linspace(-2, 6, 5);
	EbN0 = inv_dB(EbN0dB);
	Eb = Ec / code.get_rate();
	N0 = Eb * pow(EbN0, -1);
	
	berc.clear();
	
	channel.set_noise(N0(2) / 2.0);
	
	
	bvec bv;
	
	bv.set_size(BS);
	
	bv.clear();

	//cout << "bv size: " << bv.size() << endl;
	
	char_array_2_bvec(s1, 128, bv);

	
	//cout << "bv: " << bv << endl;
	
	coded_bits = code.encode(bv); //The convolutional encoder function.

	//coded_bits = coded_bits.get(0, 2 * BS - 1);

	//cout << "coded_bits size: " << bv;
	
	bpsk.modulate_bits(coded_bits, trans_symbols); //The BPSK modulator.
	
	//decoded_bits = code.decode(trans_symbols);

	//cout << "decoded_bits size: " << decoded_bits;
	
	return;
}

/*int main()
{
	epsilon2_sk esk;
	char c[128] = {'A'};
	for(int i = 0; i < 128; i++)
		c[i] = 'A';
	cout << endl;
	vec trans_symbols;
	esk.encode(c, trans_symbols);
	cout << trans_symbols << endl;
	return 0;
}*/
