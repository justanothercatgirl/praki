#include <iostream>

#include "include/prakmath.hpp"

int main() {
	prak::vector<f64> dx = {0.003, 0.005, 0.013, 0.007, 0.015, 0.020, 0.022, 0.028, 0.030, 0.032, 0.036, 0.040, 0.042, 0.046, 0.047, 0.065, 0.052};
	prak::vector<f64> ms = {0.010, 0.020, 0.050, 0.030, 0.060, 0.070, 0.080, 0.100, 0.110, 0.120, 0.130, 0.150, 0.160, 0.170, 0.180, 0.243, 0.193};
	prak::vector<f64> ss = {0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001};
	prak::pvalue<f64> A, B;
	prak::least_squares_linear<f64>(ms, dx, ss, A, B);
	std::cout << A << '\n' << B << std::endl;
	return 0;
}