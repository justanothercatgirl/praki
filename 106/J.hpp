#pragma once
#include "prakcommon.hpp"
#include <array>

inline constexpr const f64 PI = 3.141592657; // ;)

inline constexpr const f64 rho_stl = 7800; // g/dm^3 (dm = decimeter)
// delta d1 d2 d3 d4_1_ d1_ d2_ d3_ delta_
inline constexpr const std::array d_stl = 
   {0.0102, 0.0053, 0.0072, 0.0103, 0.0162, 0.0053, 0.0072, 0.0103, 0.0102};
// lambd l1 l2 l3 l4_1_ l1_ l2_ l3_ lambd_
inline constexpr const std::array l_stl = 
   {0.0024, 0.0334, 0.03515, 0.071, 0.0053, 0.033, 0.0347, 0.07345, 0.0022};

inline constexpr const f64 rho_alm = 2800; // g/dm^3 
// d4_1 D1 d4_2_ D2
inline constexpr const std::array d_alm =
   {0.01575, 0.0806, 0.01675, 0.00555};
// l4_1 L_ l4_2_ L
inline constexpr const std::array l_alm =
   {0.00635, 0.0111, 0.0053, 0.0205};


constexpr inline f64 getJ(f64 m) {
	f64 V1 = 0, V2 = 0;
	f64 m1 = 0, m2 = 0;
	f64 dm;
	f64 J1 = 0, J2 = 0;
	f64 D = d_alm.back() + d_alm[1];
	for (size_t i = 0; i < d_stl.size(); ++i)
		V1 += d_stl[i] * d_stl[i] * l_stl[i];
	for (size_t i = 0; i < d_alm.size() - 1; ++i)
		V2 += d_alm[i] * d_alm[i] * l_alm[i];
	
	m1 = PI * V1 * rho_stl / 4;
	m2 = PI * rho_alm * (V2 + (D*D - d_alm[1]*d_alm[1]) * l_alm.back()) / 4;
	dm = m - m1 - m2;
	for (size_t i = 0; i < d_stl.size(); ++i) 
		J1 += d_stl[i] * d_stl[i];
	J1 *= m1/8;

	for (size_t i = 0; i < d_alm.size(); ++i) 
		J2 += d_alm[i] * d_alm[i];
	J2 += D*D + d_alm[1]*d_alm[1];
	J2 *= m2/8;
	return J1+J2;
}
