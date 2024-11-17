#pragma once

#include <cmath>
#include <concepts>

#ifdef __x86_64__
#include <immintrin.h>
#endif

#include "prakcommon.hpp"
#include "prakmatrix.hpp"

namespace prak {

// :)
constexpr const double PI = 3.141592657;
constexpr const float fPI = 3.141592657f;

/// defines a type that supports arithmetic operation
template <typename T>
concept arithmetic = requires(T a) {
	a + a; 
	a * a; 
	a - a; 
	a / a; 
};

/// TODO: remove
enum struct operation { mul, div, add, sub };

/// vector multiply: fallback for non-floating-point types
template <arithmetic T>
void vmul(const T *op1, const T *op2, T *dest, size_t s) {
	for (size_t i = 0; i < s; ++i) {
		dest[i] = op1[i] * op2[i]; break;
	}
}

/// vector multiply: float implementation
template <>
inline void vmul(const float *op1, const float *op2, float *dest, size_t s) {
	if (s < 8) goto scalar; 
	__m256 b1;
	__m256 b2;

	for (size_t i = 0; i < s / 8; ++i) {
		b1 = _mm256_load_ps(op1 + 8*i);
		b2 = _mm256_load_ps(op2 + 8*i);
		b1 = _mm256_mul_ps(b1, b2);
		_mm256_store_ps(dest + 8*i, b1);
	}
scalar:
	for (size_t i = s - s % 8; i < s; ++i) {
		dest[i] = op1[i] * op2[i];
	}
}

/// vector multiply: double implementation
template <>
inline void vmul(const double *op1, const double *op2, double *dest, size_t s) {
	if (s < 4) goto scalar;
	__m256d b1;
	__m256d b2;
	
	for (size_t i = 0; i < s / 4; ++i) {
		b1 = _mm256_load_pd(op1 + 4*i);
		b2 = _mm256_load_pd(op2 + 4*i);
		b1 = _mm256_mul_pd(b1, b2);
		_mm256_store_pd(dest + 4*i, b1);
	}
scalar:
	for (size_t i = s - s % 4; i < s; ++i) {
		dest[i] = op1[i] * op2[i];
	}
}

/// vector division: non-floating-point types fallback
template <arithmetic T>
void vdiv(const T *op1, const T *op2, T *dest, size_t s) {
	for (size_t i = 0; i < s; ++i) {
		dest[i] = op1[i] / op2[i]; break;
	}
}

/// vector division: floating point: single precision
template <>
inline void vdiv(const float *op1, const float *op2, float *dest, size_t s) {
	if (s < 8) goto scalar;
	__m256 b1;
	__m256 b2;

	for (size_t i = 0; i < s / 8; ++i) {
		b1 = _mm256_load_ps(op1 + 8*i);
		b2 = _mm256_load_ps(op2 + 8*i);
		b1 = _mm256_div_ps(b1, b2);
		_mm256_store_ps(dest + 8*i, b1);
	}
scalar:
	for (size_t i = s - s % 8; i < s; ++i) {
		dest[i] = op1[i] / op2[i];
	}
}

/// vector division: floating point: double precision
template <>
inline void vdiv(const double *op1, const double *op2, double *dest, size_t s) {
	if (s < 4) goto scalar;
	__m256d b1;
	__m256d b2;

	for (size_t i = 0; i < s / 4; ++i) {
		b1 = _mm256_load_pd(op1 + 4*i);
		b2 = _mm256_load_pd(op2 + 4*i);
		b1 = _mm256_div_pd(b1, b2);
		_mm256_store_pd(dest + 4*i, b1);
	}
scalar:
	for (size_t i = s - s % 4; i < s; ++i) {
		dest[i] = op1[i] / op2[i];
	}
}

inline float finalize(__m256 reg) {
	// reg: x0 | x1 | x2 | x3 | x4 | x5 | x6 | x7 
	__m128 extr = _mm256_extractf128_ps(reg, 1);
	// extr: x0 | x1 | x2 | x3
	// __mm256_castps256_ps128: x4 | x5 | x6 | x7
	extr = _mm_add_ps(extr, _mm256_castps256_ps128(reg));
	// extr: x0+x4 | x1 + x5 | x2 + x6 | x3 + x7
	extr = _mm_hadd_ps(extr, extr);
	// extr: x0 + x1 + x2 + x5 | x2 + x3 + x6 x7 | ... | ...
	return extr[0] + extr[1];
}

inline double finalize(__m256d reg) {
	// reg: x0 | x1 | x2 | x3
	reg = _mm256_hadd_pd(reg, reg);
	// reg: x0 + x1 | x2 + x3 | ... | ...
	return reg[0] + reg[1];
}

/// Fallback generic method to sum a vector
template <arithmetic T>
T vsum(const T *op, size_t size) {
	T res;
	for (size_t i = 0; i < size; ++i)
		res += op[i];
	return res;
}

/// Sum a vector, float implementation
template <>
inline float vsum(const float *op, size_t size) {
	__m256 buf;
	buf = _mm256_setzero_ps();
	if (size < 8) goto scalar;
	for (size_t i = 0; i < size / 8; ++i) {
		__m256 next = _mm256_load_ps(op + 8*i);
		buf = _mm256_add_ps(buf, next);
	}
scalar:
	float linear = 0;
	for (size_t i = size - size%8; i < size; ++i) {
		linear += op[i];
	}
	return linear + finalize(buf);
}

/// Sum a vector, double implementation
template <>
inline double vsum(const double *op, size_t size) {
	__m256d buf;
	buf = _mm256_setzero_pd();
	if (size < 4) goto scalar;
	for (size_t i = 0; i < size / 4; ++i) {
		__m256d next = _mm256_load_pd(op + 4*i);
		buf = _mm256_add_pd(buf, next);
	}
scalar:
	double linear = 0;
	for (size_t i = size - size%8; i < size; ++i) {
		linear += op[i];
	}
	return linear + finalize(buf);
}

/// calculate least-squares linear approximation to fit data
/// ax+b = y (ss is an error of Y value)
template <std::floating_point T>
void least_squares_linear(
	const prak::vector<T> &xs, 
	const prak::vector<T> &ys, 
	const prak::vector<T> &ss, 
	struct pvalue<T> &a, 
	struct pvalue<T> &b)
{
	if (xs.size() != ys.size() || xs.size() != ss.size()) {
		std::cout <<   "x.size() = " << xs.size() 
			  << ", y.size() = " << ys.size()
			  << ", s.size() = " << ss.size() << std::endl;
		return;
	}
	[[assume(xs.size() == ys.size() && ys.size() == ss.size())]];
	size_t sz = xs.size();

	prak::vector<T> ones(sz);
	prak::vector<T> ssq(sz);
	prak::vector<T> ssq_m1(sz);
	prak::vector<T> buf(sz);
	prak::vector<T> buf_xs_ssq_m1(sz);
	std::fill(ones.begin(), ones.end(), (T)1); // ones: [1]
	vmul(ss.data(), ss.data(), ssq.data(), sz); // ssq: [ss*ss]
	vdiv(ones.data(), ssq.data(), ssq_m1.data(), sz); // ssq_m1: [1/ss^2]
	vmul(xs.data(), ssq_m1.data(), buf_xs_ssq_m1.data(), sz); // [xs / ss^2] 

	const T *ysd = ys.data(),
		*xsd = xs.data();
	T *ssqd = ssq.data(),
	  *ssq_m1d = ssq_m1.data(),
	  *onesd = ones.data(),
	  *bufd = buf.data(),
	  *buf_xs_ssq_m1d = buf_xs_ssq_m1.data();

	vmul(buf_xs_ssq_m1d, xsd, bufd, sz);	// buf: [xs^2 / ss^2]
	T d1 = vsum(bufd, sz); 			// sum([xs^2 / ss^2])
	T ssq_m1sum = vsum(ssq_m1d, sz);	// sum(1 / ss^2)
	vmul(xsd, ssq_m1d, bufd, sz);		// buf: [xs / ss^2]
	T d2 = vsum(buf_xs_ssq_m1d, sz);	// sum([xs / ss^2])
	T D = d1 * ssq_m1sum - d2*d2;		// sum((xs/ss)^2) * sum(1/ss^2) - sum(xs/ss^2)^2
	
	vmul(ysd, buf_xs_ssq_m1d, bufd, sz);	// buf: [ys*xs/ss^2]
	T da1 = vsum(bufd, sz);			// sum([ys*xs/ss^2])
	vmul(ysd, ssq_m1d, bufd, sz);		// buf: [ys/ss^2]
	T da2 = vsum(bufd, sz);			// sum([ys/ss^2])
	T DA = da1 * ssq_m1sum - da2 * d2;	// sum([ys*xs/ss^2]) * sum([1/ss^2]) - sum([ys/ss^2]) * sum(xs/ss^2)
	
	T DB = d1 * da2 - d2 * da1;		// sum([xs^2/ss^2]) * sum([ys/ss^2]) - sum([xs/ss^2]) * sum(ys*xs/ss^2)
	
	a.val = DA/D;
	a.err = sqrt(ssq_m1sum / D);

	b.val = DB/D;
	b.err = sqrt(d1 / D);
}


/// May throw std::bad_optional_access
template <typename T>
std::enable_if<std::is_arithmetic_v<T>, std::vector<pvalue<T>>> 
polynomial_regression(
		size_t degree, 
		std::vector<T> data_x, 
		std::vector<T> data_y, 
		std::optional<std::vector<T>> data_errors = std::nullopt)
{
	++degree; // hack)
	size_t data_size = data_x.size();
	if (data_size != data_y.size() || (data_errors.has_value() && data_errors->size() != data_size)) 
		throw dimension_error("Xs, Ys or Sigmas do not match sizes");
	struct matrix<T> X(data_size, degree),
			 Y(data_size, 1),
			 B(degree, 1),
			 W = matrix<T>::identity(data_size);
	// initialize X
	for (size_t row = 0; row < X.rows; ++row) {
		X.SUBSCR_OPRTR(row, 0) = 1;
		for (size_t col = 1; col < X.cols; ++col) {
			X.SUBSCR_OPRTR(row, col) = X.SUBSCR_OPRTR(row, col-1) * data_x[row]; 
		}
	}
	// initialize Y
	std::memcpy(Y.data(), data_y.data(), sizeof (T) * data_size);
	// initialize W
	if (data_errors.has_value()) {
		std::vector<T> &err_value = *data_errors;
		for (size_t i = 0; i < err_value.size(); ++i)
			W.data()[i * (data_size + 1)] = 1 / err_value[i] / err_value[i];
	}
	std::cerr << X << '\n' << Y << '\n' << W << '\n';
	matrix<T> X_T_W = X.tr() * W;
	matrix<T> tmp1 = (X_T_W * X).inv().value();
	B = tmp1 * X_T_W * Y;
	B.print();
	// TODO: FINISH (with covariation matrix)
	return {};
}


} // namespace prak
