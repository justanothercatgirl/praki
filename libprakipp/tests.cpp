
#include <iostream>
#include <vector>

#include "praktable.hpp"
#include "prakmath.hpp"

double avg(const std::vector<double> &v) {
	return (v[0] + v[1]) / 2 ;
}

int main_1(int argc, char *argv[]) {
	prak::table<double> t;
	std::ifstream f("test_data");
	t.read(f);
	t.column_width = 16;
	std::cout << t;
	t.apply(&avg, {"steel_n_up", "steel_n_down"}, "steel_n");
	t.apply(&avg, {"copper_n_up", "copper_n_down"}, "copper_n");
	std::cout << t;
	std::cout << "a[\"copper_n\"][5] = " << t SUBSCR_OPN "copper_n", 0 SUBSCR_CLS << std::endl;
	for (auto it = t.begin("steel_n"); it != t.end("steel_n"); ++it) {
		std::cout << *it << ' ';
	}
	auto [a, b] = t.least_squares_linear("index", "steel_n");
	std::printf("\na = %lf %lf; b = %lf %lf\n", a.val, a.err, b.val, b.err);

	return 0;
}

int main_2(int argc, char *argv[]) {
	main_1(argc, argv);
	return 0;

	prak::vector<double> x = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	prak::vector<double> y = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	prak::vector<double> s = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	struct prak::pvalue<double> val;
	prak::least_squares_linear(x, y, s, val, val);
	prak::printv(x);
	prak::vmul(x.data(), x.data(), x.data(), (size_t)x.size());
	prak::printv(x);
	return 0;
}

int main_3(int argc, char *argv[]) {
	volatile __m256 i = {10, 1, 10, 1, 10, 10, 11, 11};
	volatile __m256d j = {100, 200, -100, 400};
	printf("sum(i) = %f\n", prak::finalize(i));
	printf("sum(j) = %lf\n", prak::finalize(j));
	return 0;
}

int main_4(int argc, char *argv[]) {
	prak::vector<double> xs = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	prak::vector<double> ys = {0, 2, 2, 8, 8, 10, 13, 15, 16, 18, 20};
	prak::vector<double> ss = {1, 1, 4, 4, 1, 1, 2, 2, 1, 1, 1};
	prak::pvalue<double> a, b;
	prak::least_squares_linear(xs, ys, ss, a, b);
	std::printf("a: {%lf, %lf}; b: {%lf, %lf}\n", a.val, a.err, b.val, b.err);
	return 0;
}

int main(int argc, char *argv[]) {
	main_1(argc, argv);
}
