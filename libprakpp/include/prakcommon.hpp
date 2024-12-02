#pragma once

#include <cmath>
#include <functional>
#include <vector>
#include <iostream>

typedef void		i0;
typedef int8_t		i8;
typedef uint8_t		u8;
typedef int16_t		i16;
typedef uint16_t	u16;
typedef int32_t		i32;
typedef uint32_t	u32;
typedef int64_t		i64;
typedef uint64_t	u64;
typedef ssize_t		isz;
typedef size_t		usz;
typedef float		f32;
typedef double		f64;
#if __SIZEOF_LONG_DOUBLE__ == 16
typedef long double	f128;
#else
typedef long double	_f64;
#endif
#if defined(_MSC_VER) || !defined(__cpp_multidimensional_subscript) || __cplusplus < 202110L
#warning "can not use multidimentional subscript operator: falling back to `operator()`"
#undef MDSUBSCRIPT
#else
#define MDSUBSCRIPT
#endif

#ifdef NO_MDSUBSCRIPT
#undef MDSUBSCRIPT
#endif

#ifdef MDSUBSCRIPT
	#define SUBSCR_OPN [
	#define SUBSCR_CLS ]
	#define SUBSCR_OPRTR operator[]
#else	
	#define SUBSCR_OPN (
	#define SUBSCR_CLS )
	#define SUBSCR_OPRTR operator()
#endif

template <typename T>
using function_t = std::function<T(const std::vector<T> &)>;

namespace prak {

/// stolen from [cppreference.com](https://en.cppreference.com/w/cpp/types/numeric_limits/epsilon)
/// Compares 2 floating-point values up to `ulps` ULPS (units in the last place)
template <class T>
std::enable_if_t<not std::numeric_limits<T>::is_integer, bool>
fequal(T x, T y, std::size_t ulps = 1)
{
    // Since `epsilon()` is the gap size (ULP, unit in the last place)
    // of floating-point numbers in interval [1, 2), we can scale it to
    // the gap size in interval [2^e, 2^{e+1}), where `e` is the exponent
    // of `x` and `y`.
 
    // If `x` and `y` have different gap sizes (which means they have
    // different exponents), we take the smaller one. Taking the bigger
    // one is also reasonable, I guess.
    const T m = std::min(std::fabs(x), std::fabs(y));
 
    // Subnormal numbers have fixed exponent, which is `min_exponent - 1`.
    const int exp = m < std::numeric_limits<T>::min()
                  ? std::numeric_limits<T>::min_exponent - 1
                  : std::ilogb(m);
 
    // We consider `x` and `y` equal if the difference between them is
    // within `n` ULPs.
    return std::fabs(x - y) <= ulps * std::ldexp(std::numeric_limits<T>::epsilon(), exp);
}


/// prints a vector
template <typename T>
void printv(const T &vec) {
	std::cout << "std::vector { ";
	for (const auto &x : vec) {
		std::cout << x << ' ';
	}
	std::cout << '}' << std::endl;
}

/// An allocator that aligns memory to 64 bytes. Needed for AVX instructions.
/// C++17 required for std::align_val_t 
template <class T>
struct align_alloc : public std::allocator<T> {
	constexpr T* allocate( std::size_t n ) {
		return static_cast<T*>(::operator new(sizeof (T) * n, std::align_val_t{64}));
	}

	constexpr void deallocate( T* p, std::size_t n ) {
		::operator delete(p, std::align_val_t{64});
	}
};;

/// alias prak::vector that is the same as std::vector, but uses aligned allocator 
template <typename T>
using vector = std::vector<T, align_alloc<T>>;

/// prak value / pair value: a value with an error
template <typename T>
struct pvalue {	T val, err; };

template <typename T>
struct pvalue<T> operator*(const struct pvalue<T> &v, T a) {
	return pvalue<T>{v.val * a, v.err * a};
}

template <typename T>
std::ostream &operator<<(std::ostream &os, const struct pvalue<T> &p) {
	/* return os << "value {" << p.val << "±" << p.err << "}"; */
	return os << p.val << "±" << p.err;
}

} // namespace prak
