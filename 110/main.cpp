#include <iostream>

#include "include/praktable.hpp"

using table = prak::table<double>;
using vecarg = const std::vector<f64> &;
table data;

f64 get(std::string key) {
	return data[key, 0];
}

// [0] = x0
f64 getsqrt(vecarg v) {
	return std::sqrt(std::abs(get("x0_1") - v[0]));
}

table prepare_ex1(std::string s) {
	table ret(s);
	ret.add_column("x1", std::vector<f64>(ret.rows, NAN));
	ret.add_column("t", std::vector<f64>(ret.rows, NAN));
	ret.add_column("st", std::vector<f64>(ret.rows, NAN));
	ret.add_column("sqrt", std::vector<f64>(ret.rows, NAN));
	ret	.apply(prak::avg<f64>, {"x11", "x12", "x13"}, "x1")
		.apply(prak::avg<f64>, {"t1", "t2", "t3"}, "t")
		.apply(prak::stddev<f64>, {"t1", "t2", "t3"}, "st")
		.apply(getsqrt, {"x3"}, "sqrt");
	
	std::cout << ret;
	return ret;
}

void ex1(void) {
	table t;
}

int main() {
	data = table("common");
	prepare_ex1("data1");
	return 0;
}
