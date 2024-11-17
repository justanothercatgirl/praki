#include "prakmath.hpp"
#include "prakmatrix.hpp"

int main() {
	using namespace prak;
	matrix<double> A(2, 1, {-1, 1});
	matrix<double> B(1, 2, {1, -2});
	matrix<double> C = A*B * (1.0/1.0);
	std::cout << A << '\n' << B << '\n' << C << std::endl;
	matrix<double> D = C + matrix<double>(2, 2, {-1, -2, -3, -4});
	std::getchar();
	D.feq_precision = 10;
	matrix<double> A_1 = D.inv().value();
	std::cout << A_1 << std::endl;
	std::cout << A_1*D << D*A_1 << std::endl;
	matrix<double> E(2, 3, {1, 2, 3, 4, 5, 6});
	std::cout << E << '\n' << E.tr() << std::endl;
	std::vector<double> xs = {0, 1, 2, 3, 4, 5, 6, 7};
	std::vector<double> ys = {1, 2.9, 5.2, 7.1, 9, 11.05, 12.7, 14};
	/* std::vector<double> ss = {0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8}; */
	std::vector<double> ss = {0.8, 0.7, 0.6, 0.5, 0.4, 0.3, 0.2, 0.1};
	/* std::vector<double> xs = {0, 1, 2}; */
	/* std::vector<double> ys = {1, 2.9, 5.2}; */
	/* std::vector<double> ss = {0.1, 0.2, 0.3}; */
	polynomial_regression(2, xs, ys, std::make_optional(ss));

	matrix<double> T1 (3, 3, {1, 2, 3, 0, 1, 2, 0, 0, 1});
	matrix<double> T2 (3, 3, {4, 5, 6, 0, 4, 5, 0, 0, 6});
	std::cout << T1 << '\n' << T2 << '\n' << T1 * T1 << std::endl; 

}
