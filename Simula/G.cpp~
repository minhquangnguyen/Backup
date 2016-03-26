#include "predefine.h"
void G::matrix_multiplication(real_2d_array& a, real_2d_array& b, real_2d_array& r, ae_int_t m, ae_int_t n, ae_int_t k)
{
	alglib::smp_rmatrixgemm(m, n, k, 1, a, 0, 0, 0, b, 0, 0, 0, 0, r, 0, 0);
}

bool G::eigen_values_and_vector(real_2d_array& a, ae_int_t n, ae_int_t s, real_1d_array& rw, real_1d_array& iw, 
								real_2d_array& vr, real_2d_array& vl)
{
	bool b = alglib::rmatrixevd(a, n, s, rw, iw, vr, vl);
	return b;
}
